
#pragma once

#include "types.h"
#include "json.h"
#include "data_point.h"

namespace bot {

class CCsvLoader
{
public:
	struct SDataSet
	{
		string name;
		fs::path path;
		TDataPoints data;

		bool empty() const
		{
			return data.empty();
		}

		void clear()
		{
			name.clear();
			path.clear();
			data.clear();
		}
	};

	using TDataSets = vector<SDataSet>;

private:
	static SDataSet LoadDataSet(const fs::path& path)
	{
		SDataSet dataSet;

		if (fs::exists(path) && fs::is_regular_file(path) && path.extension() == ".csv")
		{
			std::ifstream ifstream(path.c_str());

			if (ifstream.good())
			{
				try
				{
					json::csv_parameters params;
					params.assume_header(true);
					params.header_lines(1);
					params.column_types("integer,float,float,float,float,float");
					params.ignore_empty_lines(true);
					json::json json = jsoncons::csv::decode_csv<json::json>(ifstream, params);
					const size_t lineCount = json.size();

					dataSet.data.resize(lineCount);

					for (size_t line = 0; line < lineCount; ++line)
					{
						json::json jsonColumns = json.at(line);
						SDataPoint& dataPoint = dataSet.data[line];
						const size_t columnCount = std::min(jsonColumns.size(), SDataPoint::columnCount);
						dataPoint.time = jsonColumns.at(0).as_ulonglong();

						for (size_t column = 1; column < columnCount; ++column)
						{
							dataPoint[column] = jsonColumns.at(column).as_double();
						}
					}

					dataSet.path.assign(path);
					dataSet.name.assign(utf8::to_utf8(path.filename().c_str()));
				}
				catch (const json::json_exception& e)
				{
					std::cout << e.what() << std::endl;
					dataSet.clear();
				}
			}
		}

		return dataSet;
	}

public:
	static TDataSets LoadDataSets(const string& dir)
	{
		TDataSets dataSets;
		fs::path path(dir);

		if (fs::exists(path) && fs::is_directory(path))
		{
			for (const fs::directory_iterator::value_type& entry : fs::directory_iterator(path))
			{
				SDataSet dataSet = LoadDataSet(entry.path());
				if (!dataSet.empty())
				{
					dataSets.emplace_back(std::move(dataSet));
				}
			}
		}
		return dataSets;
	}
};

} // namespace bot
