

#include "Record.h"

namespace liao::PrimedDB
{
	void Record::constructColumnView(std::unordered_map<std::string, int>& header)
	{
		for (auto& line: m_values)
		{
			for (auto& pair : header)
			{
				m_header[pair.first].emplace_back(line[pair.second]);
			}
		}
		m_abandon.resize(m_header.size());
	}
	Record::Record(std::unordered_map<std::string, int>& header, RecordData& reference)
		:m_values(std::move(reference)),m_valid(true)
	{
		constructColumnView(header);
	}
	Record::Record(Record&& move) noexcept
		:m_values(std::move(move.m_values)),m_valid(move.m_valid),m_abandon(std::move(move.m_abandon)),m_header(std::move(move.m_header)),m_byte(move.m_byte)
	{}
	void Record::operator=(Record&& move)noexcept
	{
		if (this != &move) {
			m_values = std::move(move.m_values);
			m_valid = move.m_valid;
			m_abandon = std::move(move.m_abandon);
			m_header = std::move(move.m_header);
			m_byte = move.m_byte;
		}
	}
	std::string Record::format() const
	{
		std::string result;
		if (m_valid)
		{
			for (auto& p : m_header)
				result += p.first + ":";
			result.pop_back();
			result += "\n";
			for (auto& line : m_values)
			{
				result += StaticFunc::vectorToString(line, m_abandon) + "\n";
			}
		}
		return result;
	}
	void Record::set(std::unordered_map<std::string, int>& header, RecordData& reference)
	{
		m_values = std::move(reference);
		constructColumnView(header);
		m_index = std::move(header);
		m_abandon.resize(m_values.size(), false);
		m_valid = true;
	}
	void Record::except(std::vector<std::string>& name)
	{
		for (auto& s :name)
		{
			m_abandon[m_index[s]] = true;
		}
	}
	Record& Record::where(std::vector<unsigned>& drop)
	{
		int removed = 0;
		for (int n = 0;n<m_values.size();++n)
		{
			m_values.erase(m_values.begin() + drop[n] - removed);
		}
		return *this;
	}
	Record& Record::where(std::vector<unsigned>&& drop)
	{
		int removed = 0;
		for (int n = 0; n < m_values.size(); ++n)
		{
			m_values.erase(m_values.begin() + drop[n] - removed);
		}
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
