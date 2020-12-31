#pragma once

#include <unordered_map>

struct GenericComponent
{
    unsigned int entityId;
    unsigned int componentId;

    GenericComponent *prev;
    GenericComponent *next; 

    void *data;
};

template <typename Component>
class ComponentList
{
public:
    ComponentList(GenericComponent *componentNodes) : head(componentNodes), tail(nullptr) {}
    ComponentList() : head(nullptr), tail(nullptr) {}

    class Iterator
    {
    friend class ComponentList;
    public:
        Iterator() : node(nullptr) {}

        //Pre Increment
        inline Iterator& operator++() { node = node->next; return *this; }
        
        //Post Increment
        inline Iterator operator++(int) { Iterator old = *this; node = node->next; return old; }

        inline bool operator==(const Iterator& rhs) const { return node == rhs.node; } 
        inline bool operator!=(const Iterator& rhs) const { return node != rhs.node; } 

        inline Component& operator*() const { return *(static_cast<Component* >(node->data)); }

    protected:
        Iterator(GenericComponent *componentNodes) : node(componentNodes) {}

    private:
        GenericComponent *node;
    };

    inline Iterator begin() { return head; }
    inline Iterator end() { return tail; }

private:
    Iterator head;
    Iterator tail;
};

class EntitySystem
{
public:
    EntitySystem() {};

    struct Entity
    {
    friend class EntitySystem;
        Entity(unsigned int id, Entity *previousEntity) : id(id), previousEntity(previousEntity) {}

        template <typename Component>
        inline bool has() const { return componentList.find(typeid(Component).name()) != componentList.end(); }

        template <typename Component>
        inline Component* get() { return has<Component>() ? static_cast<Component* >(componentList[typeid(Component).name()]->data) : nullptr; }

    protected:
        template <typename Component>
        inline GenericComponent* getComponent() { return has<Component>() ? componentList[typeid(Component).name()] : nullptr; }

        unsigned int id;
        Entity *previousEntity;

        std::unordered_map<std::string, GenericComponent*> componentList;
    };

    inline EntitySystem::Entity* createEntity() { EntitySystem::Entity *newEntity = new EntitySystem::Entity {nbEntity++, lastEntity}; lastEntity = newEntity; return newEntity; }

    template <typename Component>
    Component* attach(EntitySystem::Entity *entity, const Component& component);

    template <typename Component>
    void dettach(EntitySystem::Entity *entity);

    template <typename Component>
    ComponentList<Component> view();

    //template <typename FirstComponent, typename... RestComponent>
    //std::vector<Entity* > group();

private:
    unsigned int nbEntity = 0;
    Entity *lastEntity = nullptr;
    std::unordered_map<std::string, GenericComponent*> componentMap;
};

template <typename Component>
Component* EntitySystem::attach(EntitySystem::Entity *entity, const Component& component)
{
    auto id = typeid(Component).name();

    Component *cmp = new Component(component);

    if(entity->has<Component>())
    {
        entity->componentList[id]->data = cmp;
    }
    else
    {
        if(componentMap.find(id) == componentMap.end())
        {
            GenericComponent *cp = new GenericComponent {entity->id, 0, nullptr, nullptr, cmp};
            componentMap[id] = cp;

            entity->componentList[id] = cp;
        }
        else
        {
            if(componentMap[id]->prev == nullptr)
            {
                GenericComponent *cp = new GenericComponent {entity->id, 1, componentMap[id], nullptr, cmp};
                componentMap[id]->prev = cp;
                componentMap[id]->next = cp;

                entity->componentList[id] = cp;
            }
            else
            {
                GenericComponent *cp = new GenericComponent {entity->id, componentMap[id]->prev->componentId + 1, componentMap[id]->prev, nullptr, cmp};
                componentMap[id]->prev->next = cp;
                componentMap[id]->prev = cp;

                entity->componentList[id] = cp;
            }
        }
    }
    
    return cmp;
}

template <typename Component>
void EntitySystem::dettach(EntitySystem::Entity *entity)
{
    auto id = typeid(Component).name();

    if(componentMap.find(id) != componentMap.end())
    {
        GenericComponent* component = entity->getComponent<Component>();

        if(component != nullptr)
        {
            // if prev is nullptr it means that it was the only element of the list
            if(component->prev != nullptr)
            {
                if(component->next == nullptr) // if component is the last element of the list
                {
                    component->prev->next = nullptr;

                    if(component->prev == componentMap[id])
                    {
                        componentMap[id]->prev = nullptr;
                    }
                    else
                    {
                        componentMap[id]->prev = component->prev;
                    }
                }
                else
                {
                    if(component->prev != component->next) // if prev == next it means that there are only 2 elements in the list;
                    {
                        if(component->prev->next == nullptr) // it means that the current component is the first one
                        {
                            componentMap[id] = component->next;
                        }
                        else
                        {
                            component->prev->next = component->next;
                        }

                        component->next->prev = component->prev;
                    }
                    else
                    {
                        componentMap[id] = component->next;
                        component->next->prev = nullptr;
                    }
                }
            }
            else
            {
                componentMap.erase(id);
            }

            entity->componentList.erase(id);
            delete component;
        }
    }
}

template <typename Component>
ComponentList<Component> EntitySystem::view()
{
    auto id = typeid(Component).name();

    if(componentMap.find(id) != componentMap.end())
        return ComponentList<Component>(componentMap[id]);
    else
        return ComponentList<Component>();
}


