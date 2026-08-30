#pragma once

#include "external/csv.hpp"

/**
 * Create a csv::CSVReader that fails loudly on malformed data.
 *
 * The library's default policy (VariableColumnPolicy::IGNORE_ROW) silently
 * skips rows whose field count doesn't match the header, which would make
 * typos in the data files vanish without a trace. Our CSVs drive game
 * generation, so a mismatched row is an error: throw instead.
 */
inline csv::CSVReader open_csv_reader(const char* path)
{
	return csv::CSVReader(path, csv::CSVFormat::guess_csv().variable_columns(csv::VariableColumnPolicy::THROW));
}
