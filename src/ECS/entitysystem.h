#pragma once

#include <unordered_map>
#include <string>
#include <vector>

class EntitySystem
{
public:
    EntitySystem() {};

    struct GenericComponent
    {
        unsigned int entityId;

        GenericComponent *prev;
        GenericComponent *next; 

        void *data;
    };

    class Entity
    {
    friend class EntitySystem;
    public:
        Entity(unsigned int id, Entity *previousEntity) : id(id), previousEntity(previousEntity), nextEntity(nullptr) {}

        template <typename Component>
        inline bool has() const { return has(typeid(Component).name()); }

        template <typename Component>
        inline Component* get() { return has<Component>() ? static_cast<Component* >(componentList[typeid(Component).name()]->data) : nullptr; }

    protected:
        inline bool has(std::string id) const { return componentList.find(id) != componentList.end(); }

        template <typename Component>
        inline EntitySystem::GenericComponent* getComponent() { return has<Component>() ? componentList[typeid(Component).name()] : nullptr; }
        inline EntitySystem::GenericComponent* getComponent(std::string id) { return has(id) ? componentList[id] : nullptr; }

        unsigned int id;
        Entity *previousEntity;
        Entity *nextEntity;

        std::unordered_map<std::string, EntitySystem::GenericComponent*> componentList;
    };

    template <typename Component>
    class ComponentList
    {
    public:
        ComponentList(EntitySystem::GenericComponent *componentNodes) : head(componentNodes), tail(nullptr) {}
        ComponentList() : head(nullptr), tail(nullptr) {}

        class Iterator
        {
        friend class EntitySystem;
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
            Iterator(EntitySystem::GenericComponent *componentNodes) : node(componentNodes) {}

        private:
            EntitySystem::GenericComponent *node;
        };

        inline Iterator begin() { return head; }
        inline Iterator end() { return tail; }

    private:
        Iterator head;
        Iterator tail;
    };

    class GroupList
    {
    friend class EntitySystem;
    public:
        class GroupItem
        {
        friend class EntitySystem;
        friend class GroupList;
        public:
            GroupItem(unsigned int id) : entityId(id) { }

            inline bool has(std::string id) const { return componentList.find(id) != componentList.end(); }

            template <typename Component>
            inline bool has() const { return has(typeid(Component).name()); }

            template <typename Component>
            inline Component* get() { return has<Component>() ? static_cast<Component* >(componentList[typeid(Component).name()]->data) : nullptr; }

        private:
            unsigned int entityId;
            std::unordered_map<std::string, EntitySystem::GenericComponent*> componentList;

            GroupItem *next;
        };

        class Iterator
        {
        friend class GroupList;
        public:
            Iterator() : node(nullptr) {}

            //Pre Increment
            inline Iterator& operator++() { node = node->next; return *this; }
            
            //Post Increment
            inline Iterator operator++(int) { Iterator old = *this; node = node->next; return old; }

            inline bool operator==(const Iterator& rhs) const { return node == rhs.node; } 
            inline bool operator!=(const Iterator& rhs) const { return node != rhs.node; } 

            inline GroupList::GroupItem& operator*() const { return *node; }

        protected:
            Iterator(GroupList::GroupItem *componentNodes) : node(componentNodes) {}

        private:
            GroupList::GroupItem *node;
        };

        GroupList() : head(nullptr), tail(nullptr), last(nullptr), nbElement(0) {}
        GroupList(EntitySystem::GroupList::GroupItem *first, EntitySystem::GroupList::GroupItem *last, unsigned int nbElement) : head(first), tail(nullptr), last(last), nbElement(nbElement) {}

        inline Iterator begin() { return head; }
        inline Iterator end() { return tail; }

        inline unsigned int size() { return nbElement; }
        inline bool isEmpty() { return nbElement == 0; }

    private:
        Iterator head;
        Iterator tail;

        EntitySystem::GroupList::GroupItem *last;
        unsigned int nbElement;
    };

    inline EntitySystem::Entity* createEntity() { EntitySystem::Entity *newEntity = new EntitySystem::Entity {nbEntity++, lastEntity}; if(lastEntity != nullptr) { lastEntity->nextEntity = newEntity; } lastEntity = newEntity; return newEntity; }
    void removeEntity(EntitySystem::Entity* entity);

    template <typename Component>
    Component* attach(EntitySystem::Entity *entity, const Component& component);

    template <typename Component>
    void dettach(EntitySystem::Entity *entity);

    template <typename Component>
    EntitySystem::ComponentList<Component> view();

    template <typename Component, typename... Group>
    EntitySystem::GroupList* registerGroup();

    template <typename Component, typename... Group>
    EntitySystem::GroupList* group();

    //template <typename FirstComponent, typename... RestComponent>
    //std::vector<Entity* > group();

private:
    template <typename Component, typename... Args>
    auto typeToId() -> typename std::enable_if<(sizeof...(Args) == 0), std::string>::type
    {
        return std::string(typeid(Component).name()) + std::string(";");
    }

    template <typename First, typename... Next>
    auto typeToId() -> typename std::enable_if<(sizeof...(Next) > 0), std::string>::type
    {
        return typeToId<First>() + typeToId<Next...>();
    }

/*
    template <>
    std::string typeToId() { return "" }

    template <>
    bool addViewToVector(std::vector<EntitySystem::GenericComponent *> *vec)<> {}
*/
    template <typename Component, typename... Args>
    auto addViewToVector(std::vector<EntitySystem::GenericComponent *> *vec) -> typename std::enable_if<(sizeof...(Args) == 0), bool>::type
    {
        auto id = typeid(Component).name();

        if(componentMap.find(id) != componentMap.end())
        {
            vec->push_back(componentMap[id]);
            return true;
        }
        else
            return false;
    }

    template <typename First, typename... Next>
    auto addViewToVector(std::vector<EntitySystem::GenericComponent *> *vec) -> typename std::enable_if<(sizeof...(Next) > 0), bool>::type
    {
        return addViewToVector<First>(vec) && addViewToVector<Next...>(vec);
    }

    bool inline isGroupRegistered(std::string id) { return groupList.find(id) == groupList.end() ? false : true; }

    std::unordered_map<std::string, EntitySystem::GenericComponent* >::iterator dettach(EntitySystem::Entity *entity, std::string id, std::unordered_map<std::string, EntitySystem::GenericComponent* >::iterator it);

    void moveBack(EntitySystem::Entity *entity, std::string id, EntitySystem::GenericComponent *component);

    unsigned int nbEntity = 0;
    Entity *lastEntity = nullptr;
    std::unordered_map<std::string, EntitySystem::GenericComponent*> componentMap;
    std::unordered_map<std::string, EntitySystem::GroupList*> groupList;
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
            EntitySystem::GenericComponent *cp = new EntitySystem::GenericComponent {entity->id, nullptr, nullptr, cmp};
            componentMap[id] = cp;

            entity->componentList[id] = cp;
        }
        else
        {
            if(componentMap[id]->prev == nullptr)
            {
                EntitySystem::GenericComponent *cp = new EntitySystem::GenericComponent {entity->id, componentMap[id], nullptr, cmp};
                componentMap[id]->prev = cp;
                componentMap[id]->next = cp;

                entity->componentList[id] = cp;

                moveBack(entity, id, cp);
            }
            else
            {
                EntitySystem::GenericComponent *cp = new EntitySystem::GenericComponent {entity->id, componentMap[id]->prev, nullptr, cmp};
                componentMap[id]->prev->next = cp;
                componentMap[id]->prev = cp;

                entity->componentList[id] = cp;
                moveBack(entity, id, cp);
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
        EntitySystem::GenericComponent* component = entity->getComponent<Component>();

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
EntitySystem::ComponentList<Component> EntitySystem::view()
{
    auto id = typeid(Component).name();

    if(componentMap.find(id) != componentMap.end())
        return EntitySystem::ComponentList<Component>(componentMap[id]);
    else
        return EntitySystem::ComponentList<Component>();
}

template <typename Component, typename... Group>
EntitySystem::GroupList* EntitySystem::registerGroup()
{
    auto groupId = typeToId<Component, Group...>();

    if(!isGroupRegistered(groupId))
        groupList[groupId] = group<Component, Group...>();

    return groupList[groupId];
}

template <typename Component, typename... Group>
EntitySystem::GroupList* EntitySystem::group()
{
    auto groupId = typeToId<Component, Group...>();

    if(isGroupRegistered(groupId))
    {
        // return a copy of the groupList to be able to delete the pointer in the application 
 
        // provisoire
        return groupList[groupId];
    }
    else
    {
        std::vector<EntitySystem::GenericComponent *> idList;

        if(addViewToVector<Component, Group...>(&idList))
        {
            unsigned int currentEntityId = 0;
            unsigned int nbElement = 0;
            EntitySystem::GroupList::GroupItem *currentItem = nullptr;
            EntitySystem::GroupList::GroupItem *firstItem = nullptr;
            bool isFirstItem = true;
            bool sameEntityId = false; 
            bool end = false;

            std::vector<std::string> idNames;

            std::string delimiter = ";";

            size_t pos = 0;
            while ((pos = groupId.find(delimiter)) != std::string::npos)
            {
                idNames.push_back(groupId.substr(0, pos));
                groupId.erase(0, pos + delimiter.length());
            }

            idNames.push_back(groupId);

            do
            {
                do
                {
                    for (auto& node : idList)
                    {
                        if(!end)
                        {
                            while(node->entityId < currentEntityId && node->next != nullptr)
                                node = node->next;
                            
                            if(node->next == nullptr && node->entityId < currentEntityId)
                                end = true;

                            if(node->entityId > currentEntityId)
                                currentEntityId = node->entityId;
                        }                    
                    }

                    sameEntityId = true;
                    for (auto& node : idList)
                        if(node->entityId != currentEntityId)
                            sameEntityId = false;

                } while ( !end && !sameEntityId );

                if(!end)
                {
                    if(isFirstItem)
                    {
                        firstItem = new EntitySystem::GroupList::GroupItem(currentEntityId);
                        currentItem = firstItem;
                        isFirstItem = false;
                    }
                    else
                    {
                        currentItem->next = new EntitySystem::GroupList::GroupItem(currentEntityId);
                        currentItem = currentItem->next; 
                    }

                    for (int x = 0; x < idNames.size() - 1; x++)
                        currentItem->componentList[idNames[x]] = idList[x];
                        
                    nbElement++;
                    currentEntityId++;
                }

            } while (!end);
            
            return new EntitySystem::GroupList(firstItem, currentItem, nbElement);
        }
        else
        {
            return new EntitySystem::GroupList();
        }
    }

    return new EntitySystem::GroupList();
}