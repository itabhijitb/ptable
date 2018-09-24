#ifndef __PROFILER_PTMANIP_H__
#define __PROFILER_PTMANIP_H__
/*****************************************************************************/
/*                              STL Libraries                                */
/*****************************************************************************/
#include <vector>
#include <cstdlib>
namespace PTable
{
	enum Allignment { Left, Right, Center, Justify, Wrap, Default };
#define DEFINE_ALLIGNMENT(TYPE, ALLIGN)               \
		template<>                                    \
		struct DefaultAllignment<TYPE>	              \
		{                                	          \
			enum { value = ALLIGN};                   \
		};
	template<typename Ty>
	struct DefaultAllignment
	{
		enum { value = Left };
	};
	DEFINE_ALLIGNMENT(int, Right)
		DEFINE_ALLIGNMENT(float, Right)
		DEFINE_ALLIGNMENT(double, Right)
#undef DEFINE_ALLIGNMENT
		class PTManip
	{
		/* The Following Code Snippet Creates a Monoid Pattern*/
	public:
		static PTManip& Instance()
		{
			return *m_pInstance;
		}
		template<typename Ty>
		static PTManip& Instance(const Ty& data)
		{
			if (!m_destroyed)
			{
				if (m_pInstance)
				{
					//delete m_pInstance;
					m_pInstance = NULL;
				}
				m_pInstance = std::unique_ptr<PTManip>(new PTManip(data));
				std::atexit(&CleanUp);
			}
			return *m_pInstance;
		}
	private:
		static std::unique_ptr<PTable::PTManip> m_pInstance;
		static bool m_destroyed;
		static void CleanUp()
		{
			delete m_pInstance.release();
			m_pInstance = 0;
			m_destroyed = true;
		}

		/* Monoid Pattern Relevent Code Ends Here*/
	public:
		std::vector<size_t> min_col_width;
	private:

		bool is_fixed_size{}, has_header{}, has_footer{};
		bool bOvrDefaultAllign{};
		char col_sep{}, row_sep{}, header_sep{}, footer_sep{};
		Allignment OvrAllign = Allignment();


	public:
		const std::vector<size_t>& get_min_col_width()
		{
			return min_col_width;
		}

#define SET_ALLIGN_FN(what)                            \
	    void set_##what##_fn()                         \
		{	                                           \
		OvrAllign = what;                  \
		bOvrDefaultAllign = true;                      \
		} 
		SET_ALLIGN_FN(Left);
		SET_ALLIGN_FN(Right);
		SET_ALLIGN_FN(Center);
		SET_ALLIGN_FN(Default);
#undef SET_ALLIGN_FN
#define SET_SEP_FN(what)                                 \
	    void set_##what##_fn(char what)                  \
			{	                                         \
		PTManip::Instance().what = what;                 \
			} 
		SET_SEP_FN(col_sep);
		SET_SEP_FN(row_sep);
		SET_SEP_FN(header_sep);
		SET_SEP_FN(footer_sep);
#undef SET_SEP_FN
#define GET_SEP_FN(what)                      \
	    char get_##what()                     \
							{	              \
		return PTManip::Instance().what;      \
					} 
		GET_SEP_FN(col_sep);
		GET_SEP_FN(row_sep);
		GET_SEP_FN(header_sep);
		GET_SEP_FN(footer_sep);
#undef SET_SEP_FN
		bool getbOvrDefaultAllign()
		{
			return bOvrDefaultAllign;
		}
		Allignment getOvrAllign()
		{
			return OvrAllign;
		}

	private:
		PTManip() :bOvrDefaultAllign(false) {};

		PTManip(PTManip const&);
		PTManip& operator=(PTManip const&);
		template<typename Ty>
		PTManip(const Ty& data) :
			is_fixed_size(false), col_sep('|'), row_sep('-'), header_sep('_'), footer_sep('_'),
			has_header(true), has_footer(false), bOvrDefaultAllign(false)
		{
			min_col_width.erase(min_col_width.begin(), min_col_width.end());
			size_t max_container_size = 0;
			for (typename Ty::const_iterator it = data.begin();
				it != data.end();
				it++)
			{
				max_container_size = std::max(max_container_size, it->second.size() + 1);
			}
			min_col_width.resize(max_container_size, 0);
			for (typename Ty::const_iterator it = data.begin();
				it != data.end();
				it++)
			{
				std::vector<size_t>::iterator it_col_width = min_col_width.begin();
				*(it_col_width++) = std::max(min_col_width[0], it->first.size() + 2);
				for (typename Ty::value_type::second_type::const_iterator it_row = it->second.begin();
					it_row != it->second.end() && it_col_width != min_col_width.end();
					it_row++,
					it_col_width++)
				{
					*it_col_width = std::max(*it_col_width, std::max(it_row->first.size(), it_row->second.size()) + 2);
				}
			}
		}
	};
	// TEMPLATE STRUCT _Smanip
	template<class _Arg>
	struct _Smanip
	{	// store function pointer and argument value
		_Smanip(void(PTable::PTManip:: *pFun)(_Arg), _Arg val)
			: m_pFun(pFun), m_val(val)
		{	// construct from function pointer and argument value
		}

		void(PTable::PTManip:: *m_pFun)(_Arg);	// the function pointer
		_Arg m_val;	// the argument value
	};
	// TEMPLATE STRUCT _Smanip
	template<>
	struct _Smanip<void>
	{	// store function pointer and argument value
		_Smanip(void(PTable::PTManip:: *pFun)())
			: m_pFun(pFun)
		{	// construct from function pointer and argument value
		}

		void(PTable::PTManip:: *m_pFun)();	// the function pointer
	};

	template<class _Arg> inline
		std::ostream& operator<<(
			std::ostream& _Ostr, const _Smanip<_Arg>& manip)
	{	// insert by calling function with output stream and argument
		(PTManip::Instance().*(manip.m_pFun))(manip.m_val);
		return (_Ostr);
	}
	template<> inline
		std::ostream& operator<<(
			std::ostream& _Ostr, const _Smanip<void>& manip)
	{	// insert by calling function with output stream and argument
		(PTManip::Instance().*(manip.m_pFun))();
		return (_Ostr);
	}
#define SET_SEP(what)                                                    \
	static _Smanip<char>  set_##what(char sep)                           \
			{	                                                         \
		return (_Smanip<char>(&PTManip::set_##what##_fn, sep));          \
			} 
	SET_SEP(col_sep);
	SET_SEP(row_sep);
	SET_SEP(header_sep);
	SET_SEP(footer_sep);
#undef SET_SEP
#define SET_ALLIGN(what)                                                 \
	static _Smanip<void >  what##Allign()                               \
			{	                                                         \
		return (_Smanip<void>(&PTManip::set_##what##_fn));               \
			} 
	SET_ALLIGN(Left);
	SET_ALLIGN(Right);
	SET_ALLIGN(Center);
	SET_ALLIGN(Default);
#undef SET_ALLIGN
};

std::unique_ptr<PTable::PTManip> PTable::PTManip::m_pInstance = 0;
bool PTable::PTManip::m_destroyed = false;

#endif
