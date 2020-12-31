#include "entitysystem.h"

void EntitySystem::removeEntity(EntitySystem::Entity* entity)
{
    if(entity == lastEntity)
    {
        auto it = entity->componentList.begin();

        while(it != entity->componentList.end())
        {
            it = dettach(entity, it->first, it);
            it++;
        }

        lastEntity = entity->previousEntity;
        if (entity->previousEntity != nullptr)
            entity->previousEntity->nextEntity = nullptr;
    }
    else
    {
        auto it = entity->componentList.begin();

        while(it != entity->componentList.end())
        {
            if(lastEntity->has(it->first))
            {
                auto toBeChanged = entity->getComponent(it->first);
                auto toBeRemove = lastEntity->getComponent(it->first);

                toBeChanged->data = toBeRemove->data;

                dettach(lastEntity, it->first, lastEntity->componentList.find(it->first));
                lastEntity->componentList[it->first] = toBeChanged;
            }
            else
            {
                it = dettach(entity, it->first, it);
            }

            if(it != entity->componentList.end())
                it++;
        }


        EntitySystem::Entity *last;
        
        if(lastEntity->previousEntity != entity)
            last = lastEntity->previousEntity;
        else
            last = lastEntity->previousEntity->previousEntity;

        lastEntity->id = entity->id;
        lastEntity->previousEntity = entity->previousEntity;
        lastEntity->nextEntity = entity->nextEntity;
        
        if(entity->nextEntity != nullptr)
            entity->nextEntity->previousEntity = lastEntity;

        if(entity->previousEntity != nullptr)
            entity->previousEntity->nextEntity = lastEntity;

        for(auto it : lastEntity->componentList)
            if(it.second->entityId != lastEntity->id)
                moveBack(lastEntity, it.first, it.second);

        lastEntity = last;
        lastEntity->nextEntity = nullptr;
    }

    nbEntity--;
    delete entity;
}

std::unordered_map<std::string, GenericComponent* >::iterator EntitySystem::dettach(EntitySystem::Entity *entity, std::string id, std::unordered_map<std::string, GenericComponent* >::iterator it)
{
    if(componentMap.find(id) != componentMap.end())
    {
        GenericComponent* component = entity->getComponent(id);

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

            delete component;
            return entity->componentList.erase(it);
        }
    }

    return it;
}

void EntitySystem::moveBack(EntitySystem::Entity *entity, std::string id, GenericComponent *component)
{
    if(component->prev != nullptr)
    {
        if(component->prev->entityId > entity->id)
        {
            if(componentMap.find(id) != componentMap.end())
            {
                auto nextComponent = componentMap[id];

                while(nextComponent != nullptr)
                {
                    if(nextComponent->entityId > entity->id)
                    {
                        component->prev->next = component->next;

                        if(nextComponent->prev != component) // if nextComponent->prev == component it means that component was the last element
                        {
                            component->prev = nextComponent->prev;
                        }
                        else
                        {
                            component->prev->next = nullptr;
                            componentMap[id] = component;
                        }
                        component->next = nextComponent;

                        nextComponent->prev = component;
                        if(component->prev->next != nullptr)
                            component->prev->next = component;

                        component->entityId = entity->id;

                        nextComponent = nullptr;
                    }

                    if(nextComponent != nullptr)
                        nextComponent = nextComponent->next;
                }
            }
        }
    }
}