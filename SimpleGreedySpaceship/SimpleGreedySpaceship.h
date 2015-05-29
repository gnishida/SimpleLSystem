#pragma once

class SimpleGreedySpaceship {
public:
	SimpleGreedySpaceship();

	void generate();
	void draw();

private:
	void drawBox(float width, float height, float depth);
	void drawCylinder(float radius, float height, int slices = 36, bool capped = true);
};

