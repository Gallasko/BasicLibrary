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

    auto groupA = ecs.registerGroup<A, Vector>();
    auto groupB = ecs.registerGroup<A, B, Pattern, Vector>();
    auto groupC = ecs.registerGroup<B, Pattern>();

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

        std::cout << "Group A Size: " << groupA->size();
        std::cout << " Group B Size: " << groupB->size();
        std::cout << " Group C Size: " << groupC->size();

        //print3DContainer(ecs.view<Vector>(), "Printing every vector");

        for(auto j = 0; j < n; j++)
            ecs.removeEntity(entityList[indice[j]]);

        std::cout << " Group A Size: " << groupA->size();
        std::cout << " Group B Size: " << groupB->size();
        std::cout << " Group C Size: " << groupC->size() << std::endl;

        if( groupA->size() != 0 || groupB->size() != 0 || groupC->size() != 0 )
            throw -1;

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

void testViews(int indices[], int n, bool debug = false)
{
    EntitySystem ecs;

    std::vector<EntitySystem::Entity *> entityList;

    int nbA = 0, nbB = 0, nbPattern = 0, nbVector = 0;

    std::cout << "Testing set:";

    for(auto i = 0; i < n; i++)
            std::cout << " " << indices[i];

    std::cout << std::endl; 

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

    for(auto j = 0; j < n; j++)
    {
        auto i = indices[j];
        ecs.removeEntity(entityList[i]);

        if(debug)
            print3DContainer(ecs.view<Vector>(), "Printing every vector");

        nbA--;

        if(i % 2 == 0)
            nbB--;
            
        if(i % 3 == 0)
            nbPattern--;

        nbVector--;

        //std::cout << nbA << " : " << viewLen(ecs.view<A>()) << std::endl;
        if(nbA != viewLen(ecs.view<A>()))
        {
            std::cout << "View A mismatched" << std::endl;
            throw(-1);
        }

        if(nbB != viewLen(ecs.view<B>()))
        {
            std::cout << "View B mismatched" << std::endl;
            throw(-1);
        }

        if(nbPattern != viewLen(ecs.view<Pattern>()))
        {
            std::cout << "View Pattern mismatched" << std::endl;
            throw(-1);
        }

        if(nbVector != viewLen(ecs.view<Vector>()))
        {
            std::cout << "View Vector mismatched" << std::endl;
            throw(-1);
        }

        if(j % 1000 == 999)
            std::cout << "Deleted " << j << " Entities !" << std::endl;
        
    }

    print3DContainer(ecs.view<Vector>(), "Printing every vector");
}

void testGroups(int indices[], int n, bool debug = false)
{
    EntitySystem ecs;

    std::vector<EntitySystem::Entity *> entityList;

    auto groupA = ecs.registerGroup<A, Vector>();
    auto groupB = ecs.registerGroup<A, B, Pattern, Vector>();
    auto groupC = ecs.registerGroup<B, Pattern>();
    auto groupD = ecs.registerGroup<Pattern, Vector>();
    auto groupE = ecs.registerGroup<Vector>();

    int nbA = 0, nbB = 0, nbPattern = 0, nbVector = 0, nbGroupB = 0;

    std::cout << "Testing set:";

    for(auto i = 0; i < n; i++)
            std::cout << " " << indices[i];

    std::cout << std::endl; 

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

        if(i % 6 == 0)
            nbGroupB++;
            
        ecs.attach<Vector>(entity, { i, -i, 0 } );
        nbVector++;

        entityList.push_back(entity);

        if(i % 1000 == 999)
            std::cout << "Created " << i << " Entities !" << std::endl;
    }

    auto group = ecs.group<A, B, Vector>();
    
    for(auto& el : group)
    {
        el.get<B>()->a = 5;
        el.get<A>()->a = 12;
        el.get<Vector>()->z += 10;
        el.get<A>()->name = "Hut";
    }

    for(auto& element : *groupA)
    {
        element.get<A>()->a++;
        element.get<Vector>()->z += 0.25f;
    }

    for(auto j = 0; j < n; j++)
    {
        auto i = indices[j];
        ecs.removeEntity(entityList[i]);

        if(debug)
            print3DContainer(ecs.view<Vector>(), "Printing every vector");

        nbA--;

        if(i % 2 == 0)
            nbB--;
            
        if(i % 3 == 0)
            nbPattern--;

        if(i % 6 == 0)
            nbGroupB--;

        nbVector--;

        //std::cout << nbA << " : " << viewLen(ecs.view<A>()) << std::endl;
        if(nbA != viewLen(ecs.view<A>()))
        {
            std::cout << "View A mismatched" << std::endl;
            throw(-1);
        }

        if(nbB != viewLen(ecs.view<B>()))
        {
            std::cout << "View B mismatched" << std::endl;
            throw(-1);
        }

        if(nbPattern != viewLen(ecs.view<Pattern>()))
        {
            std::cout << "View Pattern mismatched" << std::endl;
            throw(-1);
        }

        if(nbVector != viewLen(ecs.view<Vector>()))
        {
            std::cout << "View Vector mismatched" << std::endl;
            throw(-1);
        }

        if(groupA->size() != nbA)
        {
            std::cout << "Group A failure" << std::endl;
            throw(-1);
        }

        if(groupB->size() != nbGroupB)
        {
            std::cout << "Group B failure" << std::endl;
            throw(-1);
        }

        if(groupC->size() != nbGroupB)
        {
            std::cout << "Group C failure" << std::endl;
            throw(-1);
        }

        if(groupD->size() != nbPattern)
        {
            std::cout << "Group D failure" << std::endl;
            throw(-1);
        }

        if(groupE->size() != nbVector)
        {
            std::cout << "Group E failure" << std::endl;
            throw(-1);
        }

        for(auto& element : *groupA)
        {
            element.get<A>()->a++;
            element.get<Vector>()->z += 0.25f;
        }

        for(auto& element : *groupB)
        {
            element.get<A>()->a++;
        }

        for(auto& element : *groupC)
        {
            element.get<B>()->b += 0.1f;
        }

        for(auto& element : *groupD)
        {
            element.get<Pattern>()->patternName = "Hello " + std::to_string(element.get<Vector>()->x);
        }

        for(auto& element : *groupE)
        {
            element.get<Vector>()->x += 0.5f;
        }

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

/*
    EntitySystem ecs;

    for(int x = 0; x < 20; x++)
    {
        auto entity = ecs.createEntity();
        ecs.attach<Vector>(entity, { x, 1, 1 } );

        if(x % 2 == 0)
        {
            ecs.attach<Position>(entity, { 0, x, 0 } );
        }

        if(x % 3 == 0)
        {
            ecs.attach<Velocity>(entity, { 0, 0, x } );
        }

        auto vecteur = entity->get<Vector>();
    } 

    //print3DContainer(ecs.view<Vector>(), "Print tout les vecteurs");

    for(auto& vector: ecs.view<Vector>())
    {
        vector.y -= 3;
    }

    print3DContainer(ecs.view<Vector>(), "Print tout les vecteurs");

    //auto fullGroup = ecs.group<Vector>();
    auto fullGroup = ecs.registerGroup<Vector, Position>();

    std::cout << fullGroup->size() << std::endl;

    auto entity = ecs.createEntity();

    ecs.attach<Vector>(entity, {0, 0, 0} );
    ecs.attach<Position>(entity, {0, 2, 4} );

    std::cout << fullGroup->size() << std::endl;

    for(auto entity : *fullGroup)
    {
        //std::cout << entity.get<Vector>()->x << std::endl;
        std::cout << entity.get<Vector>()->x << " " << entity.get<Position>()->y << std::endl;
        //std::cout << entity.get<Vector>()->x << " " << entity.get<Position>()->y << " " << entity.get<Velocity>()->z << std::endl;
    }

    for(auto& entity : *fullGroup)
    {
        entity.get<Vector>()->x += entity.get<Position>()->y;
    }

    print3DContainer(ecs.view<Vector>(), "Print tout les vecteurs");

    auto recupGroup = ecs.group<Vector, Position>();

    for(auto &entity : recupGroup)
    {
        entity.get<Vector>()->y += 15;
    }

    print3DContainer(ecs.view<Vector>(), "Print tout les vecteurs");

*/
    
    //permuteTest(8);

    //int indices[] = {1,5,0,3,2,4};

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
    int n = 10000;

    int *indice = new int [n];

    for (int i = 0; i < n; i++)
        indice[i] = i;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine e(seed);

    std::shuffle(indice, indice + n, e);

    testGroups(indice, n, false);

    //int indices[] = { 1, 3, 5, 4, 8, 6, 2, 0, 7, 9 };
    
    //testGroups(indices, 10);

/*
    int indices[] = { 0, 1, 2, 3, 4, 5, 6, 7 };

    do 
    {
        testViews(indices, 8, false);
    } while( std::next_permutation(indices, indices + 8) );
*/
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