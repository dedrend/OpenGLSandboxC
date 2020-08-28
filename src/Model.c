#include "Model.h"
#include "Mesh.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <glad/glad.h>
#include <cglm/cglm.h>
#include "Texture.h"
#include "Globals.h"

Model* create_model(Mesh *mesh)
{
    Model *entity = malloc(sizeof(Model));
    entity->mesh = mesh;
    memset(entity->position, 0, sizeof(vec3) * 2);
    entity->scale[0] = 1.f;
    entity->scale[1] = 1.f;
    entity->scale[2] = 1.f;
    return entity;
}

void draw_model(Model *entity)
{
    glBindVertexArray(entity->mesh->vertex_array_object);
    glUseProgram(entity->shader_program);

    int model_loc = glGetUniformLocation(entity->shader_program, "u_Model");
    int view_loc = glGetUniformLocation(entity->shader_program, "u_View");
    int projection_loc = glGetUniformLocation(entity->shader_program, "u_Projection");
    int texture_loc = glGetUniformLocation(entity->shader_program, "u_Texture");

    if(entity->mesh->texture)
    {
        bind_texture(entity->mesh->texture, 0);
        glUniform1i(texture_loc, 0);
    }

    mat4 model = GLM_MAT4_IDENTITY_INIT;
    glm_translate(model, entity->position);
    glm_scale(model, entity->scale);
    glm_rotate_x(model, entity->rotation[0], model);
    glm_rotate_y(model, entity->rotation[1], model);
    glm_rotate_z(model, entity->rotation[2], model);

    mat4 projection = GLM_MAT4_IDENTITY_INIT;

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, model[0]);
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, global_view[0]);
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, global_projection[0]);

    glDrawElements(GL_TRIANGLES, entity->mesh->index_count, entity->mesh->index_type, NULL);

    glBindVertexArray(0);
}