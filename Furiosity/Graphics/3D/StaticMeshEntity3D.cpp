////////////////////////////////////////////////////////////////////////////////
//  StaticMeshEntity3D.cpp
//
//  Created by Bojan Endrovski on 07/01/15.
//  Copyright (c) 2015 Bojan Endrovski. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "StaticMeshEntity3D.h"
#include "Camera3D.h"
#include "World3D.h"
#include "AssimpTools.h"

using namespace Furiosity;

StaticMeshEntity3D::StaticMeshEntity3D(World3D* world,
                                       Entity3D* parent,
                                       ModelScene3D& scene,
                                       const aiNode* node) :
    scene(scene),
    Entity3D(world, parent, -1),
    effect("/SharedResources/Shaders/Basic3D.vsh",
           "/SharedResources/Shaders/Basic3D.fsh"),
    worldViewProjParam(     *effect->GetParameter("u_worldviewproj")),
    normalMatrixParam(      *effect->GetParameter("u_normalmatrix")),
    lightDirectionParam(    *effect->GetParameter("u_lightdir")),
    ambientParam(           *effect->GetParameter("u_ambient")),
    diffuseParam(           *effect->GetParameter("u_diffuse")),
    textureParam(           *effect->GetParameter("s_texture")),
    cameraPositionParam(    *effect->GetParameter("u_camerapos")),
    attribPosition(         *effect->GetAttribute("a_position")),
    attribNormal(           *effect->GetAttribute("a_normal")),
    attribTexture(          *effect->GetAttribute("a_texture"))
{
    color       = Color::White;
    ambient     = Color::Black;
    meshIndex = node->mMeshes[0];
    
    aiMatrix4x4 t = node->mTransformation;
    SetTransformation(AssimpTools::ConvertMatrix(t));
    
    lightParams.push_back(LightShaderParameter(effect, "u_directionalLights[0]"));
    lightParams.push_back(LightShaderParameter(effect, "u_directionalLights[1]"));
    
    /*
    Matrix44 transform(t.a1, t.b1, t.c1, t.d1,
                       t.a2, t.b2, t.c2, t.d2,
                       t.a3, t.b3, t.c3, t.d3,
                       t.a4, t.b4, t.c4, t.d4);
    SetTransformation(transform);
    */
    
    aiMesh* mesh = scene->scene->mMeshes[node->mMeshes[0]];
    aiMaterial* material = scene->scene->mMaterials[mesh->mMaterialIndex];
    
    aiColor3D aiDiffuse(0.f,0.f,0.f);
    material->Get(AI_MATKEY_COLOR_DIFFUSE, aiDiffuse);
    diffuse = AssimpTools::ConvertColor(aiDiffuse);
}

void StaticMeshEntity3D::Render(RenderManager3D& renderManager)
{
    Camera3D& camera = *renderManager.activeCamera;
    
    // Use shader program
    effect->Activate();
    
    // Set light
    auto lights = renderManager.lights;
    for(int i = 0; i < lights.size() && i < lightParams.size(); i++)
        lightParams[i].SetValue(*lights[i]);
    
    ambientParam.SetValue(ambient);
    diffuseParam.SetValue(diffuse);
    cameraPositionParam.SetValue(camera.Position());
    
    Matrix44 transform = Transform();    
    
    // Set world view projection
    Matrix44 wvp = camera.Projection() * camera.View() * transform;
    worldViewProjParam.SetValue(wvp);
    
    // Set normal transformation matrix
    Matrix33 normalMtx = transform.GetMatrix33();
    normalMtx.Invert();
    normalMtx.Transpose();
    normalMatrixParam.SetValue(normalMtx);
    
    // textureParam.SetValue(*texture);
    
    ////////////////////////////////////////////////////////////////////////////////
    Mesh3D* mesh = scene->meshes[meshIndex];
    mesh->Render(attribPosition, attribNormal, attribTexture);
}

