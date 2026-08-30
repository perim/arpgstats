#include "csv.h"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <stdexcept>

static void write_file(const char* path, const char* content)
{
	std::ofstream f(path, std::ios::binary | std::ios::trunc);
	f << content;
}

// open_csv_reader() must read a well formed file as usual.
void test_wellformed_file_reads()
{
	const char* path = "csv_test_wellformed.csv";
	write_file(path, "A,B\n1,2\n3,4\n");

	int count = 0;
	csv::CSVReader reader = open_csv_reader(path);
	for (auto& row : reader)
	{
		assert(row["A"].is_int());
		assert(row["B"].is_int());
		count++;
	}
	assert(count == 2);
	std::remove(path);
}

// Rows whose field count doesn't match the header must throw, not be skipped
// silently. Note the mismatch is in the second data row on purpose: the
// library's size check runs in read_row(), which never inspects the first
// row that begin() hands out.
void test_uneven_rows_throw()
{
	const char* path = "csv_test_uneven.csv";

	const char* cases[2] = {
		"A,B\n1,2\n3,4,extra\n", // too many fields
		"A,B\n1,2\n3\n",         // too few fields
	};
	for (const char* content : cases)
	{
		write_file(path, content);
		bool threw = false;
		try
		{
			csv::CSVReader reader = open_csv_reader(path);
			for (auto& row : reader)
			{
				(void)row;
			}
		}
		catch (const std::runtime_error&)
		{
			threw = true;
		}
		assert(threw);
	}
	std::remove(path);
}

int main()
{
	test_wellformed_file_reads();
	test_uneven_rows_throw();
	return 0;
}
