////////////////////////////////////////////////////////////////////////////////
//  CollisionManager.cpp
//
//  Created by Boyan Endrovski on 7/27/11.
//  Copyright 2011 Boyan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "CollisionManager.h"

#include "CollisionMethods.h"
#include "GameWorld.h"
#include "DebugDraw2D.h"

using namespace Furiosity;


////////////////////////////////////////////////////////////////////////////////
//
//                                  Grid
//
////////////////////////////////////////////////////////////////////////////////
Grid::Grid(const std::list<Entity2D*>& entities)
{
    // Set initial values
    min.x = FLT_MAX;
    min.y = FLT_MAX;
    max.x = FLT_MIN;
    max.y = FLT_MIN;
    float maxsize = FLT_MIN;
    
    // Go over all entities for find the bounds of the world
    for(Entity2D* e : entities)
    {
        // Get the size
        float r = e->BoundingRadius();
        
        // If no collision, then skip this entitiy
        if(r <= 0)
            continue;
        
        // Track max size
        if(2 * r > maxsize)
            maxsize = 2 * r;
        
        // Build the grid size
        Vector2 pos = e->Position();
        if(pos.x + r > max.x)
            max.x = pos.x + r;
        if(pos.x - r < min.x)
            min.x = pos.x - r;
        if(pos.y + r > max.y)
            max.y = pos.y + r;
        if(pos.y - r < min.y)
            min.y = pos.y - r;
    }
    
    float sizex = max.x - min.x;
    float sizey = max.y - min.y;
    //    float gx = sizex / maxsize;
    gridx = (int)floor(sizex / maxsize);
    gridy = (int)floor(sizey / maxsize);
    
    // Resize grid
    grid.resize(gridx);
    for (int i = 0; i < gridx; i++)
        grid[i].resize(gridy);
    
    // Collect all entities
    //    Vector2 center = (min + max) * 0.5f;
    for(Entity2D* e : entities)
    {
        // Get the size
        float r = e->BoundingRadius();
        
        // If no collision, then skip this entitiy
        if(r <= 0)
            continue;
        
        // Build the grid size
        Vector2 pos = e->Position();
        pos = pos - min;
        int i = (pos.x / sizex) * gridx;
        int j = (pos.y / sizey) * gridy;
        grid[i][j].push_back(e);
    }
}

vector<Entity2D*> Grid::GetNeighbours(Entity2D &entity)
{
    vector<Entity2D*> neighbours;
    float sizex = max.x - min.x;
    float sizey = max.y - min.y;
    
    Vector2 pos = entity.Position();
    pos = pos - min;
    int idx = (pos.x / sizex) * gridx;
    int jdx = (pos.y / sizey) * gridy;
    //
    int ifrom = idx <= 0 ? 0 : idx - 1;
    int jfrom = jdx <= 0 ? 0 : jdx - 1;
    int ito = (idx >= (gridx - 1)) ? (gridx - 1) : (idx + 1);
    int jto = (jdx >= (gridy - 1)) ? (gridy - 1) : (jdx + 1);
    
    for(int i = ifrom; i <= ito; i++)
    {
        for(int j = jfrom; j <= jto; j++)
        {
            for(auto e : grid[i][j])
                neighbours.push_back(e);
        }
    }
    
    return neighbours;
}

#ifdef DEBUG
void Grid::DebugRender()
{
    float sizex = max.x - min.x;
    float sizey = max.y - min.y;
    
    float dx = sizex / gridx;
    float dy = sizey / gridy;
    float x = min.x;
    float y = min.y;
    //
    for (int i = 0; i <= gridx; i++, x += dx)
        gDebugDraw2D.AddLine(Vector2(x, min.y),
                             Vector2(x, max.y),
                             Color::Grey);
    
    for (int i = 0; i <= gridy; i++, y += dy)
        gDebugDraw2D.AddLine(Vector2(min.x, y),
                             Vector2(max.x, y),
                             Color::Grey);

    for (int i = 0; i < gridx; i++)
    {
        for (int j = 0; j < gridy; j++)
        {
            Vector2 c(i * dx, j * dy);
            c += Vector2(dx/2, dy/2);
            c += min;
            gDebugDraw2D.AddPoint(c);
            
            for(auto e : grid[i][j])
            {
                gDebugDraw2D.AddLine(e->Position(), c, Color::Grey);
            }
        }
    }
}
#endif



////////////////////////////////////////////////////////////////////////////////
// Add a pair of ids to ignore, like multiple bodies used for one 
////////////////////////////////////////////////////////////////////////////////
void CollisionManager::IgnoreByIDs(uint id0, uint id1)
{
    ignorePairs.push_back( CalcPairID(id0, id1) );
}


uint64 CollisionManager::CalcPairID(uint id0, uint id1)
{
    uint64 ret;
    if(id0 < id1)
    {
        ret = id0;
        ret = (ret << 32) | id1;        
    }
    else
    {
        ret = id1;
        ret = (ret << 32) | id0; 
    }
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
// Ignore a type of collission
////////////////////////////////////////////////////////////////////////////////
bool CollisionManager::Ignore(int type)
{
    for (int k = 0; k < ignore.size(); k++)
        if ((ignore[k] & type) == type)
        //if ((ignore[k] & type) == ignore[k])
            return true;
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// Ignore a specific collission
////////////////////////////////////////////////////////////////////////////////
bool CollisionManager::Ignore(uint id0, uint id1)
{
    // Calc pair ID
    uint64 pair = CalcPairID(id0, id1);
    
    // See if it's stored in the vactor
    for (int k = 0; k < ignorePairs.size(); k++)
        if (ignorePairs[k] == pair)
            return true;
    //
    return false;
}



////////////////////////////////////////////////////////////////////////////////
// AccumulateContacts
////////////////////////////////////////////////////////////////////////////////
void CollisionManager::AccumulateContacts(const std::list<Entity2D*>& entities)
{
    Grid grid(entities);
    //grid.DebugRender();
    
    contacts.clear();
    
    for (auto e0 : entities)
    {
        // Some have no collision radius and need no attention hence
        if(e0->BoundingRadius() == 0)
            continue;

        //
        auto neighbours = grid.GetNeighbours(*e0);
        
        for (auto e1 : neighbours)
        {
            // Skip self collision and duplicates
            if(e0->GetID() <= e1->GetID())
            //if(e0->GetID() == e1->GetID())
                continue;
            
            Contact contact;
            
            /*
            gDebugDraw2D.AddLine(e0->Position(),
                                 e1->Position());
            */
            
            //
            // Perform actual test
            if( ShapeToShape(e0->GetCollisionShape(),
                             e1->GetCollisionShape(),
                             &contact) )
            {
                // Fill up info
                contact.Resolved        = false;
                contact.Restitution     = 1.0f;
                
                // Order entities in a convinient way for use in the
                // collision events
                if(e0->EntityType() <= e1->EntityType())
                {
                    contact.FirstBody       = e0;
                    contact.SecondBody      = e1;
                }
                else
                {
                    contact.FirstBody       = e1;
                    contact.SecondBody      = e0;
                    contact.ContactNormal   *= -1.0f;
                }
                
#if DEBUG
                // Coment this out when not in use
                gDebugDraw2D.AddLine(contact.FirstBody->Position(),
                                     contact.FirstBody->Position() + contact.ContactNormal * 1.0f,
                                     Color::White);
                
                // Coment this out when not in use
                gDebugDraw2D.AddLine(contact.SecondBody->Position(),
                                     contact.SecondBody->Position() + contact.ContactNormal * -1.0f,
                                     Color::White);
#endif
                contacts.push_back(contact);
            }
        }
    }
    
    next = contacts.size();
        
    
    
    /*
    // Go over all entities
    std::list<Entity2D*>::const_iterator itr0 = entities.begin();
    int i = 0;
    for(; itr0 != entities.end() && next < maxContacts; itr0++)
    {
        i++;
        // Get pointer to entity
        Entity2D* bge0 = *itr0;
        
        // Some have no collision radius and need no attention hence
        if(bge0->BoundingRadius() == 0)
            continue;

        std::list<Entity2D*>::const_iterator itr1 = entities.begin();
        int j = 0;
        //
        // Compare with all other entities
        for(; itr1 != entities.end() && next < maxContacts; itr1++)
        {
            // Skip self testing
            j++;
            if (j <= i)
                continue;
            
            Entity2D* bge1 = *itr1;
                        
            // Some have no collision radius and need no attention hence
            if(bge1->BoundingRadius() == 0)
                continue;

            // Skip stuff on the ignore lists
            int type = bge0->EntityType() | bge1->EntityType();
            if(Ignore(type))
                continue;
            //
            if (Ignore(bge0->GetID(), bge1->GetID()))
                continue;
            
            // Do an early out test for optimization
            Vector2 normal = bge0->Position() - bge1->Position();
            // Use squre val to skip sqrt
            float distSq    = normal.SquareMagnitude();
            float rsum      = bge0->BoundingRadius() + bge1->BoundingRadius();
            float diffSq    = (rsum * rsum) - distSq;
            //
            if ( diffSq > 0 )
            {
                Contact& contact = contacts[next];
                //
                // Perform actual test
                if( ShapeToShape(bge0->GetCollisionShape(), 
                                 bge1->GetCollisionShape(),
                                 &contact) )
                {
                    // Fill up info
                    contact.Resolved        = false;
                    contact.Restitution     = 1.0f;
                    
                    // Order entities in a convinient way for use in the
                    // collision events
                    if(bge0->EntityType() <= bge1->EntityType())
                    {
                        contact.FirstBody       = bge0;
                        contact.SecondBody      = bge1;
                    }
                    else
                    {
                        contact.FirstBody       = bge1;
                        contact.SecondBody      = bge0;
                        contact.ContactNormal   *= -1.0f;
                    }
                    
#if DEBUG
                    // Coment this out when not in use
                    gDebugDraw2D.AddLine(contact.FirstBody->Position(),
                                         contact.FirstBody->Position() + contact.ContactNormal * 1.0f,
                                         Color::White);
                    
                    // Coment this out when not in use
                    gDebugDraw2D.AddLine(contact.SecondBody->Position(),
                                         contact.SecondBody->Position() + contact.ContactNormal * -1.0f,
                                         Color::White);
#endif
                    
                    // Advance
                    next++;
                } // test if
                
            } // early test if
            
        } // inner loop
        
    } // outer loop
     */
}


////////////////////////////////////////////////////////////////////////////////
// AccumulateContacts
////////////////////////////////////////////////////////////////////////////////
void CollisionManager::AccumulateContacts(const std::list<Entity2D*>& entities,
                                          const std::vector<LineSegment>&     walls)
{
    // Over all entities
    std::list<Entity2D*>::const_iterator itr = entities.begin();
    for(; itr != entities.end() && next < maxContacts; itr++)
    {
        // Get them elements
        Entity2D*         bge     = *itr;
        
        // Skip kinematic entities for optimization
        if(bge->InverseMass() == 0)
            continue;
        
        // Over all walls
        for(int j = 0; j < walls.size() && next < maxContacts; j++)
        {
            LineSegment const*      wall    = &walls[j];
            CollisionShape*         shape   = bge->GetCollisionShape();
            Contact*                contact = &contacts[next];
            
            // Run some tests
            if( CollisionShapeToLineSeg(shape, wall, contact) )
            {
                contact->FirstBody      = bge;
                contact->SecondBody     = 0;
                contact->Resolved       = false;
                contact->Restitution    = 1.0f; // TODO: Restitution
                //
                next++;
            }
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
// ResolveContacts
////////////////////////////////////////////////////////////////////////////////
void CollisionManager::ResolveContacts()
{
    // Run through all the contacts in the frame
    for(int i = 0; i < next; ++i)
    {
        Contact& contact = contacts[i];
        
        // Check if it has been handeled in the event
        if(!contact.Resolved && contact.Penetration > 0) 
        {
            // Check type of collision
            if(contact.SecondBody != 0)
            {
                Entity2D & first = *contact.FirstBody;
                Entity2D & second = *contact.SecondBody;
                
                // The movement of each object is based on inverse mass, so total that
                float totalInverseMass = first.InverseMass() + second.InverseMass();
                
                // If both have infinite mass, skip this contact
                if (totalInverseMass <= 0) continue;
                
                // Calculate amount of penetration resoluion per total inverse mass
                Vector2 movePerInverseMass = contact.ContactNormal *
                                            (-contact.Penetration / totalInverseMass);
                
                // Move 'em
                first.SetPosition( first.Position() - movePerInverseMass * first.InverseMass() );
                second.SetPosition( second.Position() + movePerInverseMass * second.InverseMass() );
            }                    
            else
            {
                // Collision with walls
                contact.FirstBody->SetPosition( contact.FirstBody->Position() +
                                               contact.ContactNormal * contact.Penetration );
            }
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
// ResolveVelocity
////////////////////////////////////////////////////////////////////////////////
void CollisionManager::ResolveVelocity()
{
    // Run through all the contacts in the frame
    for(int i = 0; i < next; ++i)
    {
        Contact& contact = contacts[i];
        
        // Get the velocity in the direction of the contact
        float separatingVelocity = contact.SeparatingVelocity();
        
        // Check if there is anything to resolve
        if(separatingVelocity > 0 || contact.Resolved || contact.VelocityResloved)
        {
            // Contact separating or stationary, move along nothing to see
            // Or gamplay does not need it to be resolved
            continue;
        }
        
        // Calculate new separating velocity
        float newSepVelocity = -separatingVelocity * contact.Restitution;
        
        float delataVelocity = newSepVelocity - separatingVelocity;
        
        // The movement of each object is based on inverse mass, so total that
        float totalInverseMass = contact.FirstBody->InverseMass();
        if(contact.SecondBody)
        {
           totalInverseMass += contact.SecondBody->InverseMass();
        }
        
        if (totalInverseMass <= 0.0f)
            continue;
        
        float impulse = delataVelocity / totalInverseMass;
        
        Vector2 impulsePerMass = contact.ContactNormal * impulse;
        
        if(contact.FirstBody && !contact.FirstBody->HasInifitesMass())
        {
            DynamicEntity2D* mv = static_cast<DynamicEntity2D*>(contact.FirstBody);
            mv->SetVelocity(mv->Velocity() + impulsePerMass * mv->InverseMass());
        }
        
        if(contact.SecondBody && !contact.SecondBody->HasInifitesMass())
        {
            DynamicEntity2D* mv = static_cast<DynamicEntity2D*>(contact.SecondBody);
            mv->SetVelocity(mv->Velocity() + impulsePerMass * -mv->InverseMass());
        }        
    }
}

////////////////////////////////////////////////////////////////////////////////
// RaiseContactEvents
////////////////////////////////////////////////////////////////////////////////
void CollisionManager::RaiseContactEvents()
{
    // Run through all the contacts in the frame
    for(int i = 0; i < next; ++i)
        gameWorld->HandleContactEvent(contacts[i]);
}


// end


