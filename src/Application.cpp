#include "Application.hpp"

#include "core/CpuImpl.hpp"
#include "core/BusImpl.hpp"
#include "core/MemoryImpl.hpp"
#include "core/GraphicsImpl.hpp"

#include "graphics/SFMLGraphicsServiceImpl.hpp"

#include "view/SFMLViewManager.hpp"
#include "view/EmulationSFMLView.hpp"

#include "facades/RomFacadeImpl.hpp"
#include "facades/RomFileInputStream.hpp"
#include "facades/SFMLGraphicsFacadeImpl.hpp"
#include "facades/InstructionExecutionFacadeImpl.hpp"

Application::Application()
{
    window.create(sf::VideoMode{320, 240, 32}, "Chip16 emulator", sf::Style::Close);
    viewManager = std::make_unique<SFMLViewManager>(window);

    auto injector = boost::di::make_injector(
        
        // Core interfaces
        boost::di::bind<Cpu>.to<CpuImpl>(),
        boost::di::bind<Bus>.to<BusImpl>(),
        boost::di::bind<Memory>.to<MemoryImpl>(),
        boost::di::bind<Graphics>.to<GraphicsImpl>(),

        // Graphics
        boost::di::bind<GraphicsService<sf::RenderTexture>>.to<SFMLGraphicsServiceImpl>(),

        // Facades
        boost::di::bind<GraphicsFacade<sf::RenderTexture>>.to<SFMLGraphicsFacadeImpl>(),
        boost::di::bind<InstructionExecutionFacade>.to<InstructionExecutionFacadeImpl>()
    );

    romFacade = injector.create<std::shared_ptr<RomFacadeImpl>>();

    std::shared_ptr<EmulationSFMLView> emulationView = injector.create<std::shared_ptr<EmulationSFMLView>>();
    viewManager->addView(emulationView);
}

void Application::run(int argc, char ** argv)
{
    std::string filename = "GB16.c16";
    if(argc > 1)
        filename = argv[1];

    bool romLoaded = romFacade->loadRomIntoMemory(std::make_shared<RomFileInputStream>(filename));
    if(!romLoaded)
        return;
    
    auto timeStart = std::chrono::high_resolution_clock::now();

    bool running = true;
    while(running)
    {
        auto now = std::chrono::high_resolution_clock::now();
        double elapsedTime = std::chrono::duration<double>(now - timeStart).count();
        timeStart = now;
        sf::Event event;
        while(window.pollEvent(event))
        {
            switch(event.type)
            {
                case sf::Event::EventType::Closed:
                    running = false;
                    break;
            }
        }

        viewManager->update(elapsedTime);
        viewManager->renderAll();
    }
    window.close();
}