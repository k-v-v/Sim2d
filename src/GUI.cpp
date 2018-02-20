/*
 * GUI.cpp
 *
 *  Created on: 20 Jan 2018
 *      Author: konstantin
 */

#include "GUI.h"
#include "Constants.h"
#include <cassert>
#include <glm/gtc/matrix_transform.hpp>

#include "GlUtil.h"
GUI::GUI() {
	// TODO Auto-generated constructor stub

}

GUI::~GUI() {
	assert(shader_ == nullptr);
	assert(VAO == 0);
	assert(VBO == 0);
	assert(Elements == 0);
}

void GUI::show(){
	ImGui::NewFrame();
	composeGUI();
	ImGui::Render();
	renderGUI();
}

//Assumes opengl is already initialized
void GUI::initialize(int screenWidth, int screenHeight, Application * app) {
	app_ = app;
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize.x = screenWidth;
	io.DisplaySize.y = screenWidth;

    io.DisplayFramebufferScale.x = 1.0f;
    io.DisplayFramebufferScale.y = 1.0f;

	io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB; // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
	io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

	//Load the gui shader
	//new Shader("basic.vert", "basic.frag");
	shader_ = new Shader("gui.vert", "gui.frag");

	shader_->use();

	//	GLuint VBO, VAO, Elements
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &Elements);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Enable the Position, UV, Color vertex attrib arrays
	glEnableVertexAttribArray(0);	//Position
	glEnableVertexAttribArray(1);	//UV
	glEnableVertexAttribArray(2);	//Color


#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF
	//TODO: Create a texture object to store the font atlast in

	//Load the default imgui atlas
	unsigned char* pixels;
	int out_bytes_per_pixel = 0; //Dummy variable to make GetTexDataAsRGBA32 work
	int width, height;

	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &out_bytes_per_pixel);
	//     MyTexture* texture = MyEngine::CreateTextureFromMemoryPixels(pixels, width, height, TEXTURE_TYPE_RGBA)
	{
		glGenTextures(1, &fontTexture_);
		glBindTexture(GL_TEXTURE_2D, fontTexture_);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	    io.Fonts->TexID = (void *)(intptr_t)fontTexture_;

		glBindTexture( GL_TEXTURE_2D, 0);	//Unbind our current texture
	}
	//Initialization done
}
void GUI::deinitialize() {
	//Clear OpenGL resources
	if (VAO)
		glDeleteVertexArrays(1, &VAO);
	if (VBO)
		glDeleteBuffers(1, &VBO);
	if (Elements)
		glDeleteBuffers(1, &Elements);
	if(fontTexture_)
		glDeleteTextures(1, &fontTexture_);

	VAO = 0;
	VBO = 0;
	Elements = 0;
	delete shader_;
}

void GUI::renderGUI() const{
    ImGuiIO& io = ImGui::GetIO();

    int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);

	ImDrawData * draw_data = ImGui::GetDrawData();


    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

	glEnable(GL_BLEND);CheckGLError();
	glBlendEquation(GL_FUNC_ADD);CheckGLError();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);CheckGLError();
	glDisable(GL_CULL_FACE);CheckGLError();
	glDisable(GL_DEPTH_TEST);CheckGLError();
	glEnable(GL_SCISSOR_TEST);CheckGLError();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);CheckGLError();

	glm::mat4 projection = {
			{ 2.0f/io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
			{ 0.0f,                  2.0f/-io.DisplaySize.y, 0.0f, 0.0f },
			{ 0.0f,                  0.0f,                  -1.0f, 0.0f },
			{-1.0f,                  1.0f,                   0.0f, 1.0f },

	};

    shader_->use();CheckGLError();
	(*shader_)["ProjMtx"] = projection;

    glBindSampler(0, 0);CheckGLError();
	glBindVertexArray(VAO);	CheckGLError();

	//Bind the atleast texture
    glActiveTexture(GL_TEXTURE0);
	GLuint texLocation = shader_->getUnirformLocation("Texture");CheckGLError();


	//Note ImGui::GetDrawData assumes we called ImGui::Render previously

	for (int n = 0; n < draw_data->CmdListsCount; n++)
	    {
	        const ImDrawList* cmd_list = draw_data->CmdLists[n];
	        const ImDrawIdx* idx_buffer_offset = 0;

	        glBindBuffer(GL_ARRAY_BUFFER, VBO);
	        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

	        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Elements);
	        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

	        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
	        {
	            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
	            if (pcmd->UserCallback)
	            {
	                pcmd->UserCallback(cmd_list, pcmd);
	            }
	            else
	            {

	                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
	                //glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
	                glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
	            }
	            idx_buffer_offset += pcmd->ElemCount;
	        }
	    }
}

void GUI::resizeWindow(int width, int height) {
	windowWidth_ = width;
	windowHeight_ = height;
}


void GUI::setMousePos(int mouseX, int mouseY) {
	//TODO::DO THIS
}
void GUI::setMouseKey(bool button1, bool button2) {
	//TODO::DO THIS
}
void GUI::setFramebufferScale(float scaleX, float scaleY){
    ImGuiIO& io = ImGui::GetIO();
    io.DisplayFramebufferScale.x = scaleX;
    io.DisplayFramebufferScale.y = scaleY;
}

/*
 * Renders the immediate mode UI
 * */
void GUI::composeGUI(){
	ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
	ImGui::Begin("Control Panel", &shouldShow_, ImGuiWindowFlags_AlwaysAutoResize);
    static int backend = 1.0f;
    ImGui::Combo("Solver backend", &backend, "Single core CPU\0Multi-core CPU\0CUDA\0\0");

    static int mode = 0;
    if(ImGui::Combo("Visualisation mode", &mode, "Velocity\0Pressure\0RHS\0FG\0\0")){
    	app_->setVisualizationMode(mode);
    }

    static float reynold = 1.0f;
    if(ImGui::SliderFloat("Reynold number", &reynold, 0.001f, 100000, nullptr, 4)){
    	app_->setReynold(reynold);
    	std::cout<<"Setting reynold to:"<<reynold<<std::endl;
    }

    static float simLength = 1.0f;
    if(ImGui::SliderFloat("Mesh length", &simLength, 0.001f, 100.0f, nullptr, 2)){
    	app_->setSimulationLength(simLength);
    	std::cout<<"Setting simLength to:"<<simLength<<std::endl;

    }
    static float deltaTime = 0.016f;

    if(ImGui::SliderFloat("Time step (s)", &deltaTime, 0.001f, 100.0f, nullptr, 2)){
    	app_->setDeltaTime(deltaTime);
    	std::cout<<"Setting deltaTime to:"<<deltaTime<<std::endl;

    }
    if(ImGui::Button("Reset")){
    	std::cout<<"Resetting\n";
    	app_->resetSimulation(reynold, simLength, deltaTime) ;
    }
    if(app_->isStable()==false){
    	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Simulation is unstable");
    }
	ImGui::End();

}
void GUI::setShouldShow(bool shouldShow){
	shouldShow_ = shouldShow;
}
void GUI::timeStep(float time){
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = time;
}

