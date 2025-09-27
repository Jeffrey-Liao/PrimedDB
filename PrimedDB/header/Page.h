#pragma once
#include <array>
#include "Global.h"
static const size_t RECORD_NUM = 500;
namespace liao::PrimedDB
{
	using page_index = unsigned short int;
	enum class PageStorageLevel
	{
		empty,
		quarter,
        half,
		lot,
		full
	};
	class Page
	{
		int& m_byteSize;
		char* m_data;
		page_index m_size;
		page_index m_used;

		void calculateUsed();
		page_index convert(page_index index)const;
	public:
		Page(char* memory, page_index size,int& byteSize);
		void remove(page_index index);
		void insert(page_index index, std::string_view data);
		char* get(page_index index);
		page_index size() const;
        page_index used() const;
		bool isFull() const;
		bool isEmpty() const;
		PageStorageLevel getLevel() const;
        ~Page();
	};
}
