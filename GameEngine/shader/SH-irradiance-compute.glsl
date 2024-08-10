#version 430 core

#define ThreadGroupX 16
#define ThreadGroupY 16

layout(local_size_x = ThreadGroupX, local_size_y = ThreadGroupY, local_size_z = 1) in;

void main () {

}