#include "game.h"

#include <vector>
#include <algorithm>
#include <random>
#include <chrono>  

struct Position
{
    float x;
    float y;
    float z;
};

struct Velocity
{
    float x;
    float y;
    float z;
};

struct A
{
    std::string name;
    int a;
};

struct B
{
    std::string name;
    int a;
    float b;
};

struct Vector
{
    float x;
    float y;
    float z;
};

struct Pattern
{
    std::string patternName;
};

template <typename Container3D>
void print3DContainer(Container3D container, std::string print = "Printing data")
{
    int i = 0;

    std::cout << print << std::endl;

    for(const auto& pos : container)
    {
        std::cout << "[" << i++ << "]: " << pos.x << " " << pos.y << " " << pos.z << std::endl;
    }
}

template <typename Container3D>
int viewLen(Container3D container)
{
    int i = 0;
    for(const auto& pos : container)
        i++;

    return i;
}

void permuteTest(int n)
{
    EntitySystem ecs;
    int indice[n];
    
    for(int i = 0; i < n; i++)
        indice[i] = i;

    EntitySystem::Entity *entityList[n];

    do
    {
        std::cout << "Testing set:";

        for(auto i : indice)
            std::cout << " " << i;

        std::cout << std::endl; 

        for(auto i = 0; i < n; i++)
        {
            auto entity = ecs.createEntity();
            ecs.attach<A>(entity, { "Entity " + std::to_string(i), i } );

            if(i % 2 == 0)
                ecs.attach<B>(entity, { "Entity " + std::to_string(i), i , i * 5.5f } );

            if(i % 3 == 0)
                ecs.attach<Pattern>(entity, { "Guess i got pattern " + std::to_string(i % 2) } );

            ecs.attach<Vector>(entity, { i, -i, indice[i] } );

            entityList[i] = entity;
        }

        //print3DContainer(ecs.view<Vector>(), "Printing every vector");

        for(auto j = 0; j < n; j++)
            ecs.removeEntity(entityList[indice[j]]);

        print3DContainer(ecs.view<Vector>(), "Printing every vector");
    } while( std::next_permutation(indice, indice + n) );
}

void testSet(int indices[], int n, bool debug = true)
{
    EntitySystem ecs;

    std::vector<EntitySystem::Entity *> entityList;

    for(auto i = 0; i < n; i++)
    {
        auto entity = ecs.createEntity();
        ecs.attach<A>(entity, { "Entity " + std::to_string(i), i } );

        if(i % 2 == 0)
            ecs.attach<B>(entity, { "Entity " + std::to_string(i), i , i * 5.5f } );

        if(i % 3 == 0)
            ecs.attach<Pattern>(entity, { "Guess i got pattern " + std::to_string(i % 2) } );

        ecs.attach<Vector>(entity, { i, -i, 0 } );

        entityList.push_back(entity);

        if(i % 1000 == 999)
            std::cout << "Created " << i << " Entities !" << std::endl;
    }

    for(auto j = 0; j < n; j++)
    {
        ecs.removeEntity(entityList[indices[j]]);

        if(debug)
            print3DContainer(ecs.view<Vector>(), "Printing every vector");

        if(j % 1000 == 999)
            std::cout << "Deleted " << j << " Entities !" << std::endl;
        
    }

    print3DContainer(ecs.view<Vector>(), "Printing every vector");
}

void testViews(int indices[], int n, bool debug = true)
{
    EntitySystem ecs;

    std::vector<EntitySystem::Entity *> entityList;

    int nbA = 0, nbB = 0, nbPattern = 0, nbVector = 0;

    for(auto i = 0; i < n; i++)
    {
        auto entity = ecs.createEntity();
        ecs.attach<A>(entity, { "Entity " + std::to_string(i), i } );
        nbA++;

        if(i % 2 == 0)
        {
            ecs.attach<B>(entity, { "Entity " + std::to_string(i), i , i * 5.5f } );
            nbB++;
        }
            
        if(i % 3 == 0)
        {
            ecs.attach<Pattern>(entity, { "Guess i got pattern " + std::to_string(i % 2) } );
            nbPattern++;
        }
            
        ecs.attach<Vector>(entity, { i, -i, 0 } );
        nbVector++;

        entityList.push_back(entity);

        if(i % 1000 == 999)
            std::cout << "Created " << i << " Entities !" << std::endl;
    }

    std::cout << "Number of A Component " << viewLen(ecs.view<A>()) << std::endl;

    for(auto j = 0; j < n; j++)
    {
        ecs.removeEntity(entityList[indices[j]]);

        if(debug)
            print3DContainer(ecs.view<Vector>(), "Printing every vector");

        if(j % 1000 == 999)
            std::cout << "Deleted " << j << " Entities !" << std::endl;
        
    }

    print3DContainer(ecs.view<Vector>(), "Printing every vector");
}

GameWindow::GameWindow(QWindow *parent) : QWindow(parent)
{
    setSurfaceType(QWindow::OpenGLSurface);
}

GameWindow::~GameWindow()
{
    delete m_device;
}

void GameWindow::render(QPainter *painter)
{
    Q_UNUSED(painter);
}

void GameWindow::initialize()
{
	initializeOpenGLFunctions();

    //permuteTest(8);

    int indices[] = {1,5,0,3,2,4};

    //int indices[] = { 0, 3, 1, 5, 7, 2, 4, 6, 8, 9 };
    
    //testSet(indices, 10);

/*
    int n = 1000000;

    int *indice = new int [n];

    for (int i = 0; i < n; i++)
        indice[i] = i;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine e(seed);

    std::shuffle(indice, indice + n, e);

    testSet(indice, n, false);
*/

    testViews(indices, 6);

    //do 
    //{

    //} while( std::next_permutation(indice, indice + n) );

    //ticking = true;
    //std::thread t (&GameWindow::tick, this);

    //t.detach();
}

void GameWindow::render()
{
    static auto currentTime = QDateTime::currentMSecsSinceEpoch();
    static auto lastTime = QDateTime::currentMSecsSinceEpoch();

    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClearColor(0.1f, 0.3f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    currentTime = QDateTime::currentMSecsSinceEpoch();
  
    lastTime = currentTime;
}

void GameWindow::setAnimating(bool animating)
{
    m_animating = animating;

    if (animating)
        renderLater();
}

void GameWindow::keyPressEvent(QKeyEvent *event)
{
}

void GameWindow::keyReleaseEvent(QKeyEvent *event)
{
}

void GameWindow::mouseMoveEvent(QMouseEvent *event)
{
}

void GameWindow::mousePressEvent(QMouseEvent *event)
{
}

void GameWindow::mouseReleaseEvent(QMouseEvent *event)
{
}

void GameWindow::renderLater()
{
    requestUpdate();
}

void GameWindow::renderNow()
{
    if (!isExposed())
        return;

    bool needsInitialize = false;

    if (!m_context) {
        m_context = new QOpenGLContext(this);
        m_context->setFormat(requestedFormat());
        m_context->create();

        needsInitialize = true;
    }

    m_context->makeCurrent(this);

    if (needsInitialize) {
        initializeOpenGLFunctions();
        initialize();
    }

    render();

    m_context->swapBuffers(this);

    if (m_animating)
        renderLater();
}

bool GameWindow::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::UpdateRequest:
        renderNow();
        return true;
    default:
        return QWindow::event(event);
    }
}

void GameWindow::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);

    if (isExposed())
        renderNow();
}

void GameWindow::tick()
{
    auto currentTime = QDateTime::currentMSecsSinceEpoch();
    auto lastTime = QDateTime::currentMSecsSinceEpoch();

    while(ticking)
    {
        lastTime = QDateTime::currentMSecsSinceEpoch();



        currentTime = QDateTime::currentMSecsSinceEpoch();
        std::this_thread::sleep_for(std::chrono::milliseconds(40 - (currentTime - lastTime)));
    }
}