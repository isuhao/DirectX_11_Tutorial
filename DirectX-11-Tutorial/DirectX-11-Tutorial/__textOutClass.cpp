#include "__textOutClass.h"

TextOutClass::TextOutClass()
{
	m_Font = 0;
	m_FontShader = 0;

	m_sentence1 = 0;
	m_sentence2 = 0;
}

TextOutClass::TextOutClass(const TextOutClass& other)
{
}

TextOutClass::~TextOutClass()
{
}

bool TextOutClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight, D3DXMATRIX baseViewMatrix)
{
	bool result;

	// Store the screen size and the base view matrix, these will be used for rendering 2D text.

	// Store the screen width and height.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Store the base view matrix.
	m_baseViewMatrix = baseViewMatrix;

	// Create and initialize the font object.

	// Create the font object.
	m_Font = new FontClass;
	if(!m_Font)
		return false;

	// Initialize the font object.
	result = m_Font->Initialize(device, "../DirectX-11-Tutorial/data/fontdata.txt", L"../DirectX-11-Tutorial/data/font.dds");
	if(!result) {
		MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the font shader object.

	// Create the font shader object.
	m_FontShader = new FontShaderClass;
	if(!m_FontShader)
		return false;

	// Initialize the font shader object.
	result = m_FontShader->Initialize(device, hwnd);
	if(!result) {
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the two strings that will be used for this tutorial.
	// One string says Hello in white at 100, 100 and the other says Goodbye in yellow at 100, 200.
	// The UpdateSentence function can be called to change the contents, location, and color of the strings at any time.

	// Initialize the first sentence.
	result = InitializeSentence(&m_sentence1, 16, device);
	if(!result)
		return false;

	// Now update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence1, "Hello", 10, 10, 1.0f, 1.0f, 1.0f, deviceContext);
	if(!result)
		return false;

	// Initialize the first sentence.
	result = InitializeSentence(&m_sentence2, 16, device);
	if(!result)
		return false;

	// Now update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence2, "Goodbye", 10, 25, 1.0f, 1.0f, 0.0f, deviceContext);
	if(!result)
		return false;

	return true;
}

// The Shutdown function will release the two sentences, the font object, and the font shader object.
void TextOutClass::Shutdown()
{
	// Release the sentences
	ReleaseSentence(&m_sentence1);
	ReleaseSentence(&m_sentence2);

	// Release the font shader object.
	if(m_FontShader) {
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	// Release the font object.
	if(m_Font) {
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;
	}
}

// Render will draw the two sentences to the screen.
bool TextOutClass::Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix)
{
	bool result;

	// Draw the first sentence.
	result = RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix);
	if(!result)
		return false;

	// Draw the second sentence.
	result = RenderSentence(deviceContext, m_sentence2, worldMatrix, orthoMatrix);
	if(!result)
		return false;

	return true;
}

// The InitializeSentence function creates a SentenceType with an empty vertex buffer which will be used to store and render sentences.
// The maxLength input parameter determines how large the vertex buffer will be.
// All sentences have a vertex and index buffer associated with them which is initialized first in this function.
bool TextOutClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Create a new sentence object.
	*sentence = new SentenceType;
	if( !*sentence )
		return false;

	// Initialize the sentence buffers to null.
	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer  = 0;

	// Set the maximum length of the sentence.
	(*sentence)->maxLength = maxLength;

	// Set the number of vertices in the vertex array.
	(*sentence)->vertexCount = 6 * maxLength;

	// Set the number of indexes in the index array.
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// Create the vertex array.
	vertices = new VertexType[(*sentence)->vertexCount];
	if(!vertices)
		return false;

	// Create the index array.
	indices = new unsigned long[(*sentence)->indexCount];
	if(!indices)
		return false;

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	// Initialize the index array.
	for(int i = 0; i < (*sentence)->indexCount; i++)
		indices[i] = i;


	// During the creation of the vertex buffer description for the sentence we set the Usage type to dynamic
	// as we may want to change the contents of the sentence at any time.

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage			= D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth		= sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags		= 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem		= vertices;
	vertexData.SysMemPitch  = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	if(FAILED(result))
		return false;


	// The index buffer is setup as a normal static buffer since the contents will never need to change.

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage			= D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth		= sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags		= D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags	= 0;
	indexBufferDesc.MiscFlags		= 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem	  = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
	if(FAILED(result))
		return false;

	// Release the vertex array as it is no longer needed.
	delete [] vertices;
	vertices = 0;

	// Release the index array as it is no longer needed.
	delete [] indices;
	indices = 0;

	return true;
}

// UpdateSentence changes the contents of the vertex buffer for the input sentence.
// It uses the Map and Unmap functions along with memcpy to update the contents of the vertex buffer.
bool TextOutClass::UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, float red, float green, float blue, ID3D11DeviceContext* deviceContext)
{
	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	// Set the color and size of the sentence.

	// Store the color of the sentence.
	sentence->red   = red;
	sentence->green = green;
	sentence->blue  = blue;

	// Get the number of letters in the sentence.
	numLetters = (int)strlen(text);

	// Check for possible buffer overflow.
	if(numLetters > sentence->maxLength)
		return false;

	// Create the vertex array.
	vertices = new VertexType[sentence->vertexCount];
	if(!vertices)
		return false;

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	// Calculate the X and Y pixel position on the screen to start drawing to.
	drawX = (float)( (m_screenWidth  /-2) + positionX );
	drawY = (float)( (m_screenHeight / 2) - positionY );

	// Build the vertex array using the FontClass and the sentence information.

	// Use the font class to build the vertex array from the sentence text and sentence draw location.
	m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	// Copy the vertex array information into the sentence vertex buffer.

	// Lock the vertex buffer so it can be written to.
	result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
		return false;

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	delete [] vertices;
	vertices = 0;

	return true;
}

// ReleaseSentence is used to release the sentence vertex and index buffer as well as the sentence itself.
void TextOutClass::ReleaseSentence(SentenceType** sentence)
{
	if( *sentence )
	{
		// Release the sentence vertex buffer.
		if((*sentence)->vertexBuffer) {
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = 0;
		}

		// Release the sentence index buffer.
		if((*sentence)->indexBuffer) {
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = 0;
		}

		// Release the sentence.
		delete *sentence;
		*sentence = 0;
	}
}

// The RenderSentence function puts the sentence vertex and index buffer on the input assembler and then calls the FontShaderClass object
// to draw the sentence that was given as input to this function.
// Notice that we use the m_baseViewMatrix instead of the current view matrix.
// This allows us to draw text to the same location on the screen each frame regardless of where the current view may be.
// Likewise we use the orthoMatrix instead of the regular projection matrix since this should be drawn using 2D coordinates.
bool TextOutClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix)
{
	unsigned int stride, offset;
	D3DXVECTOR4  pixelColor;
	bool		 result;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create a pixel color vector with the input sentence color.
	pixelColor = D3DXVECTOR4(sentence->red, sentence->green, sentence->blue, 1.0f);

	// Render the text using the font shader.
	result = m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_Font->GetTexture(), pixelColor);
	if(!result)
		false;

	return true;
}

// The SetFps function takes the fps integer value given to it and then converts it to a string.
// Once the fps count is in a string format it gets concatenated to another string so it has a prefix indicating that it is the fps speed.
// After that it is stored in the sentence structure for rendering.
// The SetFps function also sets the color of the fps string to green if above 60 fps, yellow if below 60 fps, and red if below 30 fps.
bool TextOutClass::SetFps(int fps, ID3D11DeviceContext *deviceContext)
{
	char  tempString[16];
	char  fpsString [16];
	float red, green, blue;
	bool  result;

	// Truncate the fps to below 10,000.
	if (fps > 9999)
		fps = 9999;

	// Convert the fps integer to string format.
	_itoa_s(fps, tempString, 10);

	// Setup the fps string.
	strcpy_s(fpsString, "Fps: ");
	strcat_s(fpsString, tempString);

	// If fps is 60 or above set the fps color to green.
	if (fps >= 60) {
		red   = 0.0f;
		green = 1.0f;
		blue  = 0.0f;
	}
	else {
		// If fps is below 30 set the fps color to red.
		if (fps < 30) {
			red   = 1.0f;
			green = 0.0f;
			blue  = 0.0f;
		}
		else {
			// If fps is below 60 set the fps color to yellow.
			red   = 1.0f;
			green = 1.0f;
			blue  = 0.0f;
		}
	}

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence1, fpsString, 20, 20, red, green, blue, deviceContext);
	if (!result)
		return false;

	return true;
}

// The SetCpu function is similar to the SetFps function.
// It takes the cpu value and converts it to a string which is then stored in the sentence structure and rendered.
bool TextOutClass::SetCpu(int cpu, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char cpuString [16];
	bool result;

	// Convert the cpu integer to string format.
	_itoa_s(cpu, tempString, 10);

	// Setup the cpu string.
	strcpy_s(cpuString, "Cpu: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence2, cpuString, 20, 40, 0.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
		return false;

	return true;
}
