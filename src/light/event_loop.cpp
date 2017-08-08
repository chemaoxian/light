#include <boost/assert.hpp>
#include <event2/event.h>
#include <light/event_loop.h>
#include <light/exception.h>


namespace light {

    
	event_loop::event_loop()
		:_event_base(event_base_new()) {
		
		if (_event_base == NULL) {
			throw light_exception("init event base failed");
		}
	}

	event_loop::~event_loop()
	{
		if (_event_base != NULL) {
			event_base_free(_event_base);
			_event_base = NULL;
		}
	}

	void event_loop::loop()
	{
		BOOST_ASSERT_MSG(_event_base != NULL, "call event_loop::loop() with NULL event base");

		event_base_loop(_event_base, 0);
	}

	void event_loop::quit()
	{
		BOOST_ASSERT_MSG(_event_base != NULL, "call event_loop::quit() with NULL event base");

		event_base_loopbreak(_event_base);
	}

	void event_loop::run(const Callback& callback)
	{
		BOOST_ASSERT_MSG(_event_base != NULL, "event base == NULL");

	}

	light::TimerId event_loop::run_at(const Callback& callback)
	{

	}

	light::TimerId event_loop::run_after(const Callback& callback)
	{

	}

	light::TimerId event_loop::run_every(const Callback& callback)
	{

	}

	bool event_loop::is_pedding(TimerId id)
	{

	}

	void event_loop::cancel(TimerId id)
	{

	}

	event_base* event_loop::get_event_base()
	{

	}

}