#include <iostream>
// �����������ظ�����
#include "texture.h"

#include "glfwcallback.h"
#include "vr3dcompositor.h"


static void _init_scene(vr3dscene* scene)
{
	vr3dmesh* mesh = new vr3dmesh();
	vr3dnode* node;
	vr3dshader* shader = new vr3dshader("shader/04-gltexture-v3.0.vertex", "shader/04-gltexture-v3.0.frag");

	mesh->vr_3d_mesh_new_plane(1.0f);
	node = vr3dnode::vr_3d_node_new_from_mesh_shader(mesh, shader);
	scene->vr_3d_scene_append_node(node);

	glClearColor(0.f, 0.f, 0.f, 0.f);
	//����������
	glActiveTexture(GL_TEXTURE0);

	shader->vr_3d_shader_bind();
	shader->vr_3d_shader_update_uniform_1i("tex1", 0);// ����������ԪΪ0��
}
vr3dcompositor::vr3dcompositor()
{
	this->screen_w = 960;
	this->screen_h = 540;

	this->scene = new vr3dscene(&_init_scene);
}

vr3dcompositor::~vr3dcompositor()
{
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
}

bool vr3dcompositor::vr_3d_compositor_init_scene()
{
#if 0 //��û����camera
	vr3dhmd* hmd = ((vr3dhmdcamera*)this->scene->camera)->hmd;
	if (!hmd->device)
		return false;

	screen_w = hmd->vr_3d_hmd_get_eye_width();
	screen_h = hmd->vr_3d_hmd_get_eye_height();
#endif // 0

	vr_3d_compositor_init_gl_context();

	this->scene->vr_3d_scene_init_gl();	// invoke "_init_scene" in this function

	return true;
}

bool vr3dcompositor::vr_3d_compositor_update_texture(/*AVFrame* pictureYUV*/)
{
	textureId1 = TextureHelper::load2DTexture("resources/textures/cat.png");
	glBindTexture(GL_TEXTURE_2D, textureId1);

	return true;
}

int vr3dcompositor::vr_3d_compositor_init_gl_context()
{
	// Init GLFW
	if (!glfwInit())	// ��ʼ��glfw��
	{
		std::cout << "Error::GLFW could not initialize GLFW!" << std::endl;
		return -1;
	}
	// ����OpenGL 3.3 core profile
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	glfwWindowHint(GLFW_SAMPLES, 4);				// 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//We don't want the old OpenGL
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	window = glfwCreateWindow(screen_w, screen_h, "Demo of mixing 2D texture(press A and S to adjust)", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// �����Ĵ��ڵ�contextָ��Ϊ��ǰcontext
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback_mix);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// ��glew��ȡ������չ����
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	GLenum status = glewInit();
	if (status != GLEW_OK)
	{
		std::cout << "Error::GLEW glew version:" << glewGetString(GLEW_VERSION)
			<< " error string:" << glewGetErrorString(status) << std::endl;
		glfwTerminate();
		return -1;
	}

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	// �����ӿڲ���
	glViewport(0, 0, width, height);

	return 0;
}

GLFWwindow* vr3dcompositor::vr_3d_compositor_get_window()
{
	return window;
}

bool vr3dcompositor::vr_3d_compositor_draw()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->textureId1);
	glClearColor(0.5f, 0.4f, 0.1f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	this->scene->vr_3d_scene_draw();

	glfwSwapBuffers(window); // ��������
	return true;
}