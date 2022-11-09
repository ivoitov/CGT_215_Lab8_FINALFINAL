//Written and Edited by Ivan Voitov, last edited on 11/8/22


#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>

using namespace std;
using namespace sf;
using namespace sfp;

const float KB_SPEED = 0.2;

void LoadTex(Texture& tex, string filename) {
	if (!tex.loadFromFile(filename)) {
		cout << "Won't Load" << filename << endl;
	}
}

Vector2f GetTextSize(Text text) {
	FloatRect r = text.getGlobalBounds();
	return Vector2f(r.width, r.height);
}

int main()
{
	int score(0);
	int arrows(5);

	Text arrow_count
	Text score_count
	Text game_over_text;
		
	long deltaSparrow = (0);
	
	RenderWindow window(VideoMode(800, 600), "Sparrow Hunting");
	World world(Vector2f(0, 0));

	PhysicsSprite& crossBow = *new PhysicsSprite();
	Texture cbowTex;
	LoadTex(cbowTex, "images/crossbow.png");
	crossBow.setTexture(cbowTex);
	Vector2f sz = crossBow.getSize();
	crossBow.setCenter(Vector2f(400, 600 - (sz.y / 2)));

	PhysicsSprite arrow;
	Texture arrowTex;
	LoadTex(arrowTex, "images/arrow.png");
	arrow.setTexture(arrowTex);
	bool drawingArrow(false);

	PhysicsRectangle top;
	top.setSize(Vector2f(800, 10));
	top.setCenter(Vector2f(400, 5));
	top.setStatic(true);
	world.AddPhysicsBody(top);

	PhysicsRectangle right;
	right.setSize(Vector2f(10, 600));
	right.setCenter(Vector2f(795, 300));
	right.setStatic(true);
	world.AddPhysicsBody(right);

	// PhysicsRectangle left;
	// left.setSize(Vector2f(10, 600));
	// left.setCenter(Vector2f(5, 300));
	// left.setStatic(true);
	// world.AddPhysicsBody(left);

	Texture redTex;
	LoadTex(redTex, "images/sparrow.png");
	PhysicsShapeList<PhysicsSprite> sparrows;
	for (int i(0); i < 8; i++) {
		PhysicsSprite& sparrow = sparrows.Create();
		sparrow.setTexture(redTex);
		int x = 50 + ((850 / 5) * i);
		Vector2f sz = sparrow.getSize();
		sparrow.setCenter(Vector2f(x, 20 + (sz.y / 2)));
		sparrow.setVelocity(Vector2f(0.1, 0)); //change x back to .25
		world.AddPhysicsBody(sparrow);
		sparrow.onCollision = [&drawingArrow, &world, &arrow, &sparrow, &sparrows, &score](PhysicsBodyCollisionResult result) {
		if (result.object2 == arrow) {
			drawingArrow = false;
			world.RemovePhysicsBody(arrow);
			world.RemovePhysicsBody(sparrow);
			sparrows.QueueRemove(sparrow);
			score += 100;
		}
		if (result.object2 == right) {
			world.RemovePhysicsBody(sparrow);
			sparrows.QueueRemove(sparrow);
		}
		};
	}

	top.onCollision = [&drawingArrow, &world, &arrow]
	(PhysicsBodyCollisionResult result) {
		drawingArrow = false;
		world.RemovePhysicsBody(arrow);
	};

	Font fnt;
	if (!font.loadFromFile("images/arial.ttf")) {
		cout << "Couldn't load font" << endl;
		exit(1);
	}
	score_count.setFont(fnt);
	arrow_count.setFont(fnt);
	
	Clock clock;
	Time lastTime(clock.getElapsedTime());
	Time currentTime(lastTime);
	while ((arrows > 0) || drawingArrow) {
		currentTime = clock.getElapsedTime();
		Time deltaTime = currentTime - lastTime;
		long deltaMS = deltaTime.asMilliseconds();

		if (deltaMS > 9) {
			lastTime = currentTime;
			world.UpdatePhysics(deltaMS);
			MoveCrossbow(crossBow, deltaMS);
			if (Keyboard::isKeyPressed(Keyboard::Space) && !drawingArrow) {
				drawingArrow = true;
				arrows = arrows - 1;
				arrow.setCenter(crossBow.getCenter());
				arrow.setVelocity(Vector2f(0, -1));
				world.AddPhysicsBody(arrow);
			}

			window.clear();
			if (drawingArrow) {
				window.draw(arrow);
			}
			sparrows.DoRemovals();
			for (PhysicsShape& sparrow : sparrows) {
				window.draw((PhysicsSprite&)sparrow);
			}

			window.draw(crossBow);
			score_count.setString(to_string(score));
			FloatRect text_bounds = score_count.getGlobalBounds();
			score_count.setPosition(Vector2f(790 - text_bounds.width, 590 - text_bounds.height));
			window.draw(score_count);
			arrow_count.setString(to_string(arrows));
			text_bounds = arrow_count.getGlobalBounds();
			arrow_count.setPosition(Vector2f(10, 590 - text_bounds.height));
			window.draw(arrow_count);
			//world.VisualizeAllBounds(window);

			window.display();
			sparrows.DoRemovals();
		}
	}
	window.display(); // this is needed to see the last frame
	game_over_text.setString("GAME OVER");
	game_over_text.setFont(fnt);
	sz = GetTextSize(game_over_text);
	game_over_text.setPosition(400 - (sz.x / 2), 300 - (sz.y / 2));
	window.draw(game_over_text);
	window.display();
	
	bool leave(true);
	do {
		if (Keyboard::isKeyPressed(Keyboard::Space)) {
			leave = false;
		}
	} while (leave);
}