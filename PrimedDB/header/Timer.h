#pragma once
#include <chrono>
namespace liao::Util
{
	template<class TimeType>
	class Timer
	{
		TimeType m_timer;
		bool m_started;
		static auto now()
		{
			return std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch());
		}
	public:
		Timer()
			:m_timer(now())
		{
			start();
		}
		Timer(TimeType time)
			:m_timer(time)
		{
			start();
		}
		bool isStarted()const
		{
			return m_started;
		}
		void start()
		{
			m_timer = now();
		}
		void end()
		{
			m_timer = now() - m_timer;
		}
		bool isEmpty()const
		{
			return m_timer == TimeType(0);
		}
        TimeType get()
        {
			auto timer = m_timer;
			m_timer = TimeType(0);
			m_started = false;
            return timer;
        }
		static TimeType Get()
		{
			return now();
		}
	};
}
