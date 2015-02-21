////////////////////////////////////////////////////////////////////////////////
//  StaticMeshEntity3D.h
//  Furiosity
//
//  Created by Bojan Endrovski on 07/01/15.
//  Copyright (c) 2015 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Entity3D.h"
#include "Renderer3D.h"
#include "ModelScene3D.h"
#include "Effect.h"
#include "Texture.h"

namespace Furiosity
{
    class StaticMeshEntity3D : public Entity3D, public Renderable3D
    {
        
        Effect              effect;
        
        ShaderParameter&    worldViewProjParam;
        ShaderParameter&    normalMatrixParam;
        ShaderParameter&    lightDirectionParam;
        ShaderParameter&    cameraPositionParam;
        ShaderParameter&    diffuseParam;
        ShaderParameter&    ambientParam;
        ShaderParameter&    textureParam;
        
        vector<LightShaderParameter> lightParams;
        
        ShaderAttribute&    attribPosition;
        ShaderAttribute&    attribNormal;
        ShaderAttribute&    attribTexture;
        
        Texture*            texture;
        
        ModelScene3D&       scene;
        
        int meshIndex;
        
        Color color;
        Color ambient;
        Color diffuse;
        
    public:
        StaticMeshEntity3D(World3D*         world,
                           Entity3D*        parent,
                           ModelScene3D &    scene,
                           const aiNode*    node);
        
        virtual void Render(RenderManager3D& renderManager) override;
    };
}


