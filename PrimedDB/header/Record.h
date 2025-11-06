#pragma once

#include "Defs.h"
#include "Global.h"

namespace liao::PrimedDB {
	class Record
	{
		using RecordData = std::deque<std::deque<std::shared_ptr<std::string>>>;
		using ColumnView = std::deque<std::shared_ptr<std::string >>;
		using RowLine = std::deque<std::shared_ptr<std::string >>;
		std::unordered_map<std::string, ColumnView> m_header;
		std::unordered_map<std::string, int> m_index;
		RecordData m_values;
		unsigned m_byte;
		bool m_valid = false;
		std::vector<bool> m_abandon;
		void constructColumnView(std::unordered_map<std::string, int>&);
	public:
		Record() = default;
		Record(std::unordered_map<std::string,int>&, RecordData&);
		Record(Record&&)noexcept;
		void set(std::unordered_map<std::string, int>&, RecordData&);
		void except(std::vector<std::string>&);
		Record& where(std::vector<unsigned>& drop);
		Record& where(std::vector<unsigned>&& drop);
		//void where(std::string&, std::function<bool>);
		ColumnView& at(std::string columnName);
		RowLine& at(unsigned index);
		bool isValid() const;
		std::string format() const;
		void operator=(Record&&)noexcept;
	};
}
