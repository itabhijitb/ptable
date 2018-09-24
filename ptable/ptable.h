#ifndef __PROFLIER_PTABLE_H__
#define __PROFLIER_PTABLE_H__
/*****************************************************************************/
/*                              STL Libraries                                */
/*****************************************************************************/
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <set>
/*****************************************************************************/
/*                            PROJECT Libraries                              */
/*****************************************************************************/
#include "ptmanip.h"
namespace std {
	/*
	C++ Standard does not allow to_string on a string. This is an important addition for cases, which require to 
	write a generic code to work on various data types assuming those types would be casted to a string
	*/
	std::string to_string(const std::string& value)
	{
		return value;
	}
}
namespace PTable
{

	struct HEADER {};
	struct DETAIL {};
	struct SEP {};
	struct LINE {};
	struct TITLE {};
	template<typename Ty, Allignment>
	struct Allign {
		std::string operator()(Ty elem, size_t size)
		{
			std::string st = std::to_string(elem);
			if (size < st.size())
				st.erase(st.begin() + size, st.end());
			std::stringstream ss_cell;
			ss_cell
				<< std::left
				<< std::setw(std::max(size, st.size()))
				<< st;
			return ss_cell.str();
		}
	};
	template<typename Ty>
	struct Allign<Ty, Center> {
		std::string operator()(Ty elem, size_t size)
		{
			std::stringstream ss_cell;
			std::string st = std::to_string(elem);
			if (size < st.size())
				st.erase(st.begin() + st.size() - size, st.end());
			size_t padding = (size - st.size()) / 2;
			ss_cell
				<< std::string(padding, ' ')
				<< st
				<< std::string(size - padding - st.size(), ' ');
			return ss_cell.str();
		}
	};
	template<typename Ty>
	struct Allign<Ty, Right> {
		std::string operator()(Ty elem, size_t size)
		{
			std::string st = std::to_string(elem);
			if (size < st.size())
				st.erase(st.begin() + st.size() - size, st.end());
			std::stringstream ss_cell;
			size_t padding = (size - st.size());
			ss_cell
				<< std::string(padding, ' ')
				<< st;
			return ss_cell.str();
		}
	};
	template<typename Ty>
	class Cell
	{
	private:

	public:
		Cell(Ty value,
			size_t size,
			char col_sep = PTManip::Instance().get_col_sep()
		) :m_value(value), m_size(size), m_col_sep(col_sep) {}
		friend std::ostream& operator<<(std::ostream& os, const Cell& cell)
		{
			std::stringstream st;
			Allignment allign = static_cast<Allignment>(DefaultAllignment<Ty>::value);
			if (PTManip::Instance().getbOvrDefaultAllign())
			{
				allign = PTManip::Instance().getOvrAllign();
			}
			st << cell.m_col_sep;
			switch (allign)
			{
			case Left:
				st << Allign<Ty, Left>()(cell.m_value, cell.m_size);
				break;
			case Right:
				st << Allign<Ty, Right>()(cell.m_value, cell.m_size);
				break;
			case Center:
				st << Allign<Ty, Center>()(cell.m_value, cell.m_size);
				break;
			default: break;
			}
			st << cell.m_col_sep;
			os << st.str();
			return os;
		}
	private:
		Ty m_value;
		size_t m_size;
		char m_col_sep;
	};
	class RowBase
	{
	public:
		RowBase()
		{
			m_total_width
				= std::accumulate(
					PTManip::Instance().get_min_col_width().begin(),
					PTManip::Instance().get_min_col_width().end(),
					0ll)
				+ PTManip::Instance().get_min_col_width().size() + 1;
		};
		friend std::ostream& operator<<(std::ostream& os, const RowBase& row);
		virtual RowBase& Header()
		{
			return *this;
		}
		virtual RowBase& Detail()
		{
			return *this;
		}
		virtual ~RowBase() {};
	protected:
		typedef std::vector<size_t> width_type;
		std::string st;
		size_t m_total_width;
	};
	std::ostream& operator<<(std::ostream& os, const RowBase& row)
	{
		os << std::endl << row.st;
		return os;
	}
	template<typename  Ty>
	class Row : public RowBase
	{
	public:
		typedef Ty value_type;
		Row(value_type row)
		{
			std::stringstream strm;
			width_type::const_iterator it_width = PTManip::Instance().min_col_width.begin();
			for (typename value_type::const_iterator it = row.begin();
				it != row.end();
				it++, it_width++)
			{
				strm << Cell<Ty>(*it, *it_width);
			}
			st = strm.str();
		}
	};
	template<typename  T1, typename T2>
	class Row<std::pair<const T1, std::map<T1, T2> > > : public RowBase
	{
		typedef std::pair<const T1, std::map<T1, T2> > value_type;
	public:
		Row(value_type row) :m_row(row) {};
		RowBase& Header()
		{
			width_type::const_iterator it_width = PTManip::Instance().get_min_col_width().begin();
			std::stringstream strm;
			strm << Cell<std::string>(" ", *(it_width++));
			for (typename value_type::second_type::const_iterator it = m_row.second.begin();
				it != m_row.second.end();
				it++, it_width++)
			{
				strm << Cell<T1>(it->first, *it_width);
			}
			st = strm.str();
			return *this;
		}
		RowBase& Detail()
		{

			std::vector<size_t>::const_iterator it_width = PTManip::Instance().get_min_col_width().begin();
			std::stringstream strm;
			strm << Cell<std::string>(m_row.first, *(it_width++));
			for (typename value_type::second_type::const_iterator it = m_row.second.begin();
				it != m_row.second.end() && it_width != PTManip::Instance().get_min_col_width().end();
				it++, it_width++)
			{
				strm << Cell<T2>(it->second, *it_width);
			}
			st = strm.str();
			return *this;
		}
	private:
		value_type m_row;
	};
	template<>
	class Row < SEP > : public RowBase
	{
	public:
		Row(char row_sep = PTManip::Instance().get_row_sep(),
			char col_sep = PTManip::Instance().get_col_sep())
		{
			std::stringstream strm;
			for (width_type::const_iterator it_width = PTManip::Instance().get_min_col_width().begin();
				it_width != PTManip::Instance().get_min_col_width().end();
				++it_width)
			{
				strm << Cell<std::string>(std::string(*it_width, row_sep), *it_width, col_sep);
			}
			st = strm.str();
		}

	};
	template<>
	class Row < LINE > : public RowBase
	{
	public:
		Row(char row_sep = PTManip::Instance().get_row_sep(),
			char col_sep = PTManip::Instance().get_col_sep())
		{

			std::stringstream strm;
			strm << Cell<std::string>(
				std::string(m_total_width, row_sep),
				m_total_width,
				col_sep);
			st = strm.str();
		}

	};
	template<>
	class Row < TITLE > : public RowBase
	{
	public:
		Row(std::string title)
		{
			std::stringstream strm;
			strm << Cell<std::string>(title, m_total_width);
			st = strm.str();
		}

	};
	template<typename Ty>
	class Table
	{
	public:
		Table(Ty table) : m_table(table) { }
		friend std::ostream& operator<<(std::ostream& os, const Table<Ty>& tbl1)
		{
			Table<Ty> _tbl = tbl1;
			std::stringstream st;
			st << Row<SEP>(
				PTManip::Instance().get_header_sep(),
				PTManip::Instance().get_header_sep());
			typename Ty::iterator header_row_it = _tbl.m_table.begin();
			for (typename Ty::iterator it = _tbl.m_table.begin();
				it != _tbl.m_table.end();
				it++)
			{
				if (header_row_it->second.size() < it->second.size())
				{
					header_row_it = it;
				}
			}

			//for (typename Ty::iterator it = _tbl.m_table.begin();
			//	it != _tbl.m_table.end();
			//	it++)
			//{

			//	for (typename Ty::const_iterator::value_type::second_type::const_iterator jt = header_row_it->second.begin();
			//		jt != header_row_it->second.end();
			//		jt++)
			//	{
			//		if (it->second.count(jt->first) == 0)
			//		{
			//			it->second[jt->first] = "N/A";
			//		}
			//	}
			//}

			st << Row<typename Ty::value_type >(*header_row_it).Header();
			st << Row<SEP>();
			for (typename Ty::iterator it_row = _tbl.m_table.begin();
				it_row != _tbl.m_table.end();
				it_row++)
			{
				st << Row<typename Ty::value_type >(*it_row).Detail();
				st << Row<SEP>(
					std::distance(it_row, _tbl.m_table.end()) == 1
					? PTManip::Instance().get_footer_sep()
					: PTManip::Instance().get_row_sep()
					);
			}
			os << st.str();
			return os;
		}
	private:
		Ty m_table;
		typedef Ty TableTy;
		typedef typename Ty::const_iterator TableIter;
	};
}
#endif
