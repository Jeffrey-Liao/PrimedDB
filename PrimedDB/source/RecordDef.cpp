

#include "Record.h"

namespace liao::PrimedDB
{
	void Record::constructColumnView(std::unordered_map<std::string, int>& header)
	{
		m_title.resize(header.size());
		for (auto& pair : header)
		{
			m_title[pair.second] = pair.first;
		}
		for (auto& line : m_values)
		{
			for (auto& pair : header)
			{
				m_header[pair.first].emplace_back(line[pair.second]);
			}
		}
		m_include.resize(m_title.size(), false);
	}
	Record::Record(bool valid)
		:m_valid(valid)
	{
		
	}
	Record::Record(std::unordered_map<std::string, int>& header, RecordData& reference)
		:m_values(std::move(reference)),m_valid(true)
	{
		constructColumnView(header);
	}
	Record::Record(Record&& move) noexcept
		:m_values(std::move(move.m_values)),m_valid(move.m_valid),m_include(std::move(move.m_include)),m_header(std::move(move.m_header)),m_byte(move.m_byte),m_title(std::move(move.m_title)), m_available(std::move(move.m_available))
	{}
	void Record::operator=(Record&& move)noexcept
	{
		if (this != &move) {
			m_values = std::move(move.m_values);
			m_valid = move.m_valid;
			m_include = std::move(move.m_include);
			m_available = std::move(move.m_available);
			m_title = std::move(move.m_title);
			m_header = std::move(move.m_header);
			m_byte = move.m_byte;
		}
	}
	std::string Record::format() const
	{
		std::string result;
		if (m_valid)
		{
			for (int n = 0 ;n< m_title.size();++n)
			{
				if (m_include[n])
					result += m_title[n] + ":";
			}

			if (!result.empty())
				result.pop_back();
			result += "\n";
			for (int n = 0;n< m_values.size();++n)
			{
				if (m_available.empty() || m_available[n])
					result += StaticFunc::vectorToString(m_values[n], m_include) + "\n";
			}
		}
		return result;
	}
	void Record::set(std::unordered_map<std::string, int>& header, RecordData& reference)
	{
		m_values = std::move(reference);
		constructColumnView(header);
		m_valid = true;
	}
	void Record::include(std::string& name)
	{
		auto iter = std::find(m_title.begin(),m_title.end(),name);
		if (iter != m_title.end())
		{
			m_include[iter - m_title.begin()] = true;
		}
	}
	Record& Record::all()
	{
		std::fill(m_include.begin(), m_include.end(), true);
		return *this;
	}
	Record& Record::where(std::vector<bool>& avaliable)
	{
		m_available = std::move(avaliable);
		return *this;
	}
	Record::ColumnView& Record::at(std::string columnName)
	{
		return m_header.at(columnName);
	}
	Record::RowLine& Record::at(unsigned index)
	{
		return m_values.at(index);
	}
	//void Record::where(std::vector<std::string>&, std::function<bool>);
	//void Record::where(std::string&, std::function<bool>);
	bool Record::isValid() const
	{
		return m_valid;
	}
}
