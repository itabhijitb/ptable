#include "gtest/gtest.h"
#include <cmath>
#include "../ptable/ptable.h"
TEST(TestCaseName, TestName) {
	const double pi = std::acos(-1);
	typedef std::map<std::string, std::map<std::string, std::string> > TyTable;
	typedef std::map<std::string, std::map<std::string, double> > TyTable1;
	TyTable table
	{
		{"Sin" , { {"pi/6", std::to_string(sin(pi/6))}, {"pi/4", std::to_string(sin(pi / 4))}, {"3*pi/4", std::to_string(sin(3 * pi / 4))}, {"pi/2", std::to_string(sin(pi / 2))}}},
		{"Cos" , { {"pi/6", std::to_string(cos(pi / 6))}, {"pi/4", std::to_string(cos(pi / 4))}, {"3*pi/4", std::to_string(cos(3 * pi / 4))}, {"pi/2", std::to_string(cos(pi / 2))}}},
		{"Tan" , { {"pi/6", std::to_string(tan(pi / 6))}, {"pi/4", std::to_string(tan(pi / 4))}, {"3*pi/4", std::to_string(tan(3 * pi / 4))}, {"pi/2", std::to_string(tan(pi / 2))}}},
	};
	PTable::PTManip::Instance(table);
	std::cout << PTable::CenterAllign();
	std::cout << PTable::Row<PTable::LINE>(PTable::PTManip::Instance().get_header_sep(), ' ');
	std::cout << PTable::Row<PTable::TITLE>("Trigonometry result for First Quadrant");
	std::cout << PTable::Row<PTable::LINE>(PTable::PTManip::Instance().get_footer_sep());
	std::cout << PTable::Table<TyTable>(table);
	TyTable1 table1 {
		{"Sin" , { {"2*pi/6", sin(pi / 6)}, {"2*pi/4", sin(pi / 4)}, {"2*3*pi/4", sin(3 * pi / 4)}, {"2*pi/2", sin(pi / 2)}}},
		{"Cos" , { {"2*pi/6", cos(pi / 6)}, {"2*pi/4", cos(pi / 4)}, {"2*3*pi/4", cos(3 * pi / 4)}, {"2*pi/2", cos(pi / 2)}}},
		{"Tan" , { {"2*pi/6", tan(pi / 6)}, {"2*pi/4", tan(pi / 4)}, {"2*3*pi/4", tan(3 * pi / 4)}, {"2*pi/2", tan(pi / 2)}}},
	};
	PTable::PTManip::Instance(table);
	std::cout << PTable::CenterAllign();
	std::cout << PTable::Row<PTable::LINE>(PTable::PTManip::Instance().get_header_sep(), ' ');
	std::cout << PTable::Row<PTable::TITLE>("Trigonometry result for Second Quadrant");
	std::cout << PTable::Row<PTable::LINE>(PTable::PTManip::Instance().get_footer_sep());
	std::cout << PTable::Table<TyTable1
	>(table1);
  EXPECT_TRUE(true);
}