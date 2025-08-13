//#pragma once
//#include <exception>
//#include <string>
//namespace liao::PrimeDB {
//	class LiaoException : public std::exception {
//	public:
//		LiaoException() : m_message("") {}
//		LiaoException(const char* message) : m_message(message) {}
//		LiaoException()
//		LiaoException(std::string message) : m_message(message) {}
//		const char* what() const noexcept override {
//			return m_message.c_str();
//		}
//	private:
//		std::string m_message;
//	};
//}