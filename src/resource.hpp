#ifndef RESOURCE_HPP_
#define RESOURCE_HPP_
#ifdef _WIN32
#include <GL/gl3w.h>
#endif

#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>

#include <AL/al.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <notify.hpp>

#include <map>
#include <vector>
#include <string>

#include "errorhandler.hpp"
#include "shader.hpp"
#include "script.hpp"

class Resource
{
public:
    Resource()
    {
        failed = true;
    }
    virtual ~Resource() {}
    virtual int load(const char *filename)
    {
        return 0;
    }

    bool failed;
};

class TextureResource : public Resource
{
public:
    uint32_t id;
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
    uint32_t type;

    TextureResource()
    {
        id     = -1;
        width  = 0;
        height = 0;
        bpp    = 0;
        type   = 0;

        glGenTextures(1, &id);
    }
    ~TextureResource()
    {
        glDeleteTextures(1, &id);
    }
};

class SoundResource : public Resource
{
public:
    uint32_t buffer;
    uint32_t format;
    int64_t freq;

    SoundResource()
    {
        buffer = -1;
        freq = 0;
        format = 0;

        alGenBuffers(1, &buffer);
    }
    ~SoundResource()
    {
        alDeleteBuffers(1, &buffer);
    }
};

class ShaderResource : public Resource
{
public:
    std::vector<Shader *> parents;
    uint32_t handle;

    ShaderResource()
    {
        handle = 0;
    }
    ~ShaderResource()
    {
        for(auto p : parents) {
            p->unload();
        }

        parents.clear();

        if(handle) {
            glDeleteShader(handle);
        }
    }
};

class ModelResource : public Resource
{
public:
    int num_tris;

    struct {
        glm::vec3 max;
        glm::vec3 min;
    } bounding_box;

    void updateBoundingBox(glm::vec3 vertex);

    uint32_t vertex_buffer;
    uint32_t indices_buffer;
    uint32_t uv_buffer;
    uint32_t normals_buffer;

    ModelResource()
    {
        bounding_box.min = bounding_box.max = glm::vec3(0,0,0);

        num_tris = 0;
        glGenBuffers(1, &vertex_buffer);
        glGenBuffers(1, &indices_buffer);
        glGenBuffers(1, &uv_buffer);
        glGenBuffers(1, &normals_buffer);
    }
    ~ModelResource()
    {
        glDeleteBuffers(1, &vertex_buffer);
        glDeleteBuffers(1, &indices_buffer);
        glDeleteBuffers(1, &uv_buffer);
        glDeleteBuffers(1, &normals_buffer);
    }
};

class ScriptResource : public Resource
{
public:
    asIScriptModule *module;

    ScriptResource()
    {
        module = 0;
    }
    ~ScriptResource()
    {
        if(module) {
            module->Discard();
        }
    }
};

class ResourceHandler
{
public:
    ResourceHandler();
    ~ResourceHandler();

    int init();
    void update();

    TextureResource *getTexture(const char *filename);
    ModelResource *getModel(const char *filename);
    ShaderResource *getShader(Shader *parent, const char *filename);
    ScriptResource *getScript(const char *filename);
    SoundResource *getSound(const char *filename);
private:
    Notify notify;

    std::map<std::string, Resource *> resources;

    Resource *getResource(const char *filename);
    Resource *getByType(const char *ext);

    char datapath[FILENAME_MAX];
    char enginepath[FILENAME_MAX];
};

#endif // RESOURCE_HPP_
