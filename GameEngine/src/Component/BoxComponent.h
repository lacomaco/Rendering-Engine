#pragma once
struct BoxComponent {
	int width;
	int height;

	BoxComponent(int width = 0, int height = 0) {
		this->width = width;
		this->height = height;
	}
};