#include <chrono>
#include <cxxopts.hpp>
#include <egt/detail/string.h>
#include <egt/ui>
#include <sstream>
#include <string>

int main(int argc, char** argv)
{
	bool playing = false;
    egt::Application app(argc, argv);
#ifdef EXAMPLEDATA
    egt::add_search_path(EXAMPLEDATA);
#endif
	
	egt::Size size(320, 240);
    auto format = egt::detail::enum_from_string<egt::PixelFormat>("yuyv");
    auto dev("/dev/video0");

    egt::TopWindow win;
    win.background(egt::Image("file:background.jpg"));

    egt::CameraWindow player(size, dev, format, egt::WindowHint::overlay);
    player.move_to_center(win.center());
    win.add(player);

    const auto wscale = static_cast<float>(egt::Application::instance().screen()->size().width()) / size.width();
    const auto hscale = static_cast<float>(egt::Application::instance().screen()->size().height()) / size.height();

	player.device(dev);
	player.move(egt::Point(0, 0));
	player.scale(wscale, hscale);
	
    // wait to start playing the video until the window is shown
    win.on_show([&player, &playing]()
    {
        player.start();
		playing = true;
    });

    player.on_connect([&player, &dev](const std::string & devnode)
    {
			std::cout << devnode << "is connected: starting it" << std::endl;
    });

	player.on_disconnect([&player, &playing, dev](const std::string & devnode)
    {
        if (player.device() == devnode)
        {
            std::cout << devnode << "is disconnected: stoping it" << std::endl;
            player.stop();
			playing = false;
        }
    });
	
	// handle win event
	win.on_event([&player, &playing](egt::Event & event)
	{
        std::cout << event.key().keycode;
		
		if (event.id() == egt::EventId::keyboard_down)
            std::cout << " down";
        else if (event.id() == egt::EventId::keyboard_up)
		{
            std::cout << " up";
			
			if( playing )
			{
				player.hide();
				player.stop();
				playing = false;
			}
			else 
			{
				player.start();
				playing = true;
				player.show();
			}
		}
        else if (event.id() == egt::EventId::keyboard_repeat)
            std::cout << " repeat";
		
		std::cout << std::endl;
	}, {egt::EventId::keyboard_down,
        egt::EventId::keyboard_up,
        egt::EventId::keyboard_repeat
       });
	
    player.show();
    win.show();

    return app.run();
}
