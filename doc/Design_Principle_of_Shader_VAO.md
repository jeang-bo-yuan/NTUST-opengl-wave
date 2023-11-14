# Design Principles of Shader and VAO

## Shader

- 使用版本： `#version 430 core`

- Vertex Shader的attribute遵守下列順序

|Index|Type|Content|
|-----|----|-------|
|0    |vec3|頂點的座標|

- Vertex Shader必需包含下列Uniform Data

|Type|Name|Description|
|--- |--- |---        |
|mat4|view_matrix|將世界座標轉成視空間座標的矩陣|
|mat4|proj_matrix|將視空間轉成clip coordinate的矩陣|
|uint|frame|目前是第幾幀|

- Fragment Shader必需包含下列Uniform Data

|Type|Name|Description|
|--- |--- |---        |
|vec3|eye_position|眼睛的位置|

## VAO

- 子類別在覆寫`VAO_Interface::draw`時，不該呼叫`glUseProgram`，使用的Shader要由呼叫者自行決定