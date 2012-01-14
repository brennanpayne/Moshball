/*
	Implementation of a list of Targets.
	Also does most operations that deal with the whole list, including collision.

	CS 559 Project 2 (Fall 2011)

	Brennan Payne
	Nick Svoboda

*/

#include "targetList.h"
#include "rangeRand.h"




struct TargetList::closestWall{
	double howFar;
	Vec3 collisionPoint;
	bool hasBeenHit;
};


TargetList::TargetList(){
	//Nothing happens here... Also I shouldn't ever use it, so why did I put it in here?
}


TargetList::TargetList(int s, int seed, freetype::font_data font){

	our_font = font;
	srand(seed);
	size = s;

	//How much time until a target switches back to 'not hit' state
	startTime = 30+size*2;

	closestIndex = -1;
	closestDistance = -1;

	closestTarget = new Target();

	MakeList();
}

TargetList::TargetList(const TargetList &other){
	size = other.size;
	startTime = other.startTime;

	//TODO- don't need index
	closestIndex = other.closestIndex;
	closestDistance = other.closestDistance;
	closestTarget = other.closestTarget;

	head = other.head;
}

/**
	I'm not entirely sure this works right, but it's probably close. Should double-check later.
**/
TargetList::~TargetList(){
	Target *tmp;

	while(head->getNext() != NULL){
		tmp = head;	
		delete head;

		head = tmp->getNext();
	}

	delete tmp;
}


/*
	Possibile place to do some bonus work:
	Poisson distribution (don't let 2 targets exist in the same place)

	http://en.wikipedia.org/wiki/Poisson_distribution

	poisson random number (Knuth):
		init:
			L <- e(power -lambda), k<-0, p <-1
		do:
			k = k+1
			find random number u (0-1), p=p*u
		while(p>L)
		return k-1
    
*/
double poisson(double lambda){
	double L = exp(-lambda);
	double p = 1.0, k = 0;

	do{
		k++;
		p *= rangeRand(0, 1);
	}while(p < L);

	return k - 1;
}//except this doesn't work (no bounds on number)


/*
	Create the list of Targets
	-working
*/
void TargetList::MakeList(){
	Vec3 pos;
	Target *curr, *last;
	bool first = true;

	for(int i = 0; i < size; i++){

		//targets exist somewhere within the walls (not closer than 50 units to the wall)
		pos = Vec3(rangeRand(-2590, 2590), 0, rangeRand(-2590, 2590));

		if(first){
			curr = new Target(pos, startTime);
			head = curr;
			first = false;
			last = curr;
		}
		else{
			curr = new Target(pos, startTime, last);
			last->setNext(curr);
			last = curr;
		}
	}
}

/*
Check collisions between the player and target spheres.
http://www.gamasutra.com/view/feature/3015/pool_hall_lessons_fast_accurate_.php?page=1
-very helpful runthrough of things that need to be done. 
Attempting to translate that into actual code. (Actually they have code as well I didn't see right away)

Should return true if it hits something. Also sets closestDistance when it has hit something
that's closer than whatever was in closestDistance before.
*/
bool TargetList::checkCircles(Vec3 target, double velocity, double timeSinceLast){

	Vec3 psn = player->getPosition();
	Vec3 direction = player->getViewDir(); //* -1;

	//how far we're trying to move in this time period (center of player sphere)
	double distanceGoing = velocity * timeSinceLast;

	//Non-normalized direction vector
	Vec3 dirAndDist = direction * distanceGoing;

	//vector between player and target
	Vec3 between = target - psn;

	double betwLength = between.magnitude();
	//Player isn't moving far enough in this time period to hit the target, no collision
	if(distanceGoing < betwLength -100.0){
		return false;
	}

	//Player isn't moving towards this object, so no collision.
	
	if(between.dotProd(direction) <= 0){
		return false;
	}
	

	//SHOULD ALREADY BE NORMALIZED, BUT THIS IS A GOOD DOUBLE CHECK
	Vec3 norm = direction.normalize();

	double distBetween = norm.dotProd(between);

	//This line of code is now officially my least favorite line ever.
	double F = distBetween*distBetween - betwLength*betwLength;

	//Player doesn't pass close enough to target to hit a glancing collision.
	if(F >= 100*100){
		return false;
	}

	//And then I realized that the example I was looking at had actual pre-written code, continues:

	// We now have F and sumRadii, two sides of a right triangle.
	// Use these to find the third side, sqrt(T)
	double T = 100 * 100 - F;

	// If there is no such right triangle with sides length of
	// sumRadii and sqrt(f), T will probably be less than 0.
	// Better to check now than perform a square root of a
	// negative number.
	if(T < 0){
		return false;
	}

	// Therefore the distance the circle has to travel along
	// distMoved is D - sqrt(T)
	double dist = distBetween - sqrt(T);

	// Get the magnitude of the movement vector
	//double mag = distMoved.magnitude();

	// Finally, make sure that the distance A has to move
	// to touch B is not greater than the magnitude of the
	// movement vector.
	if(distanceGoing < dist){
		return false;
	}

	//the first collision we find must be the closest.
	if(closestDistance == -1.0){
		closestDistance = dist;
		return true;
	}
	//..until we find a closer one
	else if(abs(closestDistance) > abs(dist)){
		closestDistance = dist;
		return true;
	}

	//We've already found a collision that happens before this one. (like a collision with a wall)
	//not that that makes much sense, but hey.
	return false; 
}



void TargetList::checkCollisions(CCamera *p, double velocity, double timeSinceLast){

	//we can't really check for collisions if we're not moving anywhere
	if(timeSinceLast <= 0 || velocity == 0) return;

	double totalDistance = velocity * timeSinceLast;

	player = p;

	Target *curr = head;
	bool hit = false;

	double distance = velocity * timeSinceLast;
	//bool hitWall = checkWalls(velocity, timeSinceLast);

	closestWall wall = checkWalls(velocity, timeSinceLast);

	for(int i=0; i < size; i++){

		hit = checkCircles(curr->position, velocity, timeSinceLast);

		//Don't un-hit a ball that was already hit (the timer within Target will do that)
		if(hit){
			//closestIndex = i;
			closestTarget = curr;
			curr ->setHit(true);
		}

		//update countdown on any hit ball
		curr->computeTime(timeSinceLast);

		curr = curr->getNext();
	}

	if(closestTarget->isHit()){

		double timeUsed = abs(closestDistance / velocity);

		//			Vec3 distToMove = closestTarget->getPosition() - player->getPosition();

		//we might be inside the hit sphere (yes I know we weren't supposed to do it this way
		if(timeUsed > timeSinceLast){
			timeSinceLast = 0;
		}
		else timeSinceLast -= timeUsed;

		//NOTICE THIS, ME
		player->MoveForward(closestDistance);

		//point on the sphere that we're hitting
		Vec3 collisionPoint = player->getPosition() + player->getViewDir()*50;


		Vec3 sphereNormal = collisionPoint - closestTarget->getPosition();
		sphereNormal = sphereNormal.normalize();

		player->Reflect(sphereNormal);
		double remainingDistance = totalDistance - closestDistance;

		if(velocity < 0){
			player->MoveForward(-1*remainingDistance);
		}
		else
			player->MoveForward(remainingDistance);
		closestDistance = -1.0;


		//if(timeUsed > timeSinceLast)
		//checkCollisions(player, velocity, timeSinceLast);

		closestTarget = new Target();
	}

	/*
	//TODO- make wall collision work!
	else if(wall.hasBeenHit){
	Vec3 normal = wall.collisionPoint - player->getPosition();
	normal = normal.normalize();

	player->Reflect(normal);

	double remainingDistance = totalDistance - wall.howFar;
	player->MoveForward(remainingDistance);
	closestDistance = -1.0;
	}
	*/

	else{
		//we didn't find any collisions, so move forward the whole distance.
		player->MoveForward(totalDistance);
	}

	//start the checking over again, doesn't matter what this session found
	closestDistance = -1;
	closestIndex = -1;
}


/*
Taken from: http://devmag.org.za/2009/04/17/basic-collision-detection-in-2d-part-2/ :
*/
//May return a point and if it's 1 its the collision point, 0 is no collision (ie null), and 2 just take the average
Vec3 TargetList::circleLineCollision(Vec3 lineP1, Vec3 lineP2, Vec3 CircleCenter, double distMoving){
	Vec3 LocalP1 = lineP1 - CircleCenter;
	Vec3 LocalP2 = lineP2 - CircleCenter;

	Vec3 P2MinusP1 = LocalP2 - LocalP1;

	double a = (P2MinusP1.x) * (P2MinusP1.x) + (P2MinusP1.y) * (P2MinusP1.y);
	double b = 2 * ((P2MinusP1.x * LocalP1.x) + (P2MinusP1.y * LocalP1.y));
	//Radius of 50 is the hardcoded value
	double c = (LocalP1.x * LocalP1.x) + (LocalP1.y * LocalP1.y) - (50.0 * 50.0);

	double delta = (b*b);
	delta -= (4*a*c);

	if (delta < 0){ // No intersection
		//Vec3 temp;
		hitAWall = false;
		return Vec3();
	}
	else if (delta == 0){ // One intersection
		double u = -b / (2 * a);
		//Vec3 temp((u * P2MinusP1.x) , (u * P2MinusP1.y), (u * P2MinusP1.z));
		//return lineP1 + temp;
		//double distance = ((temp.x - CircleCenter.x) * (temp.x - CircleCenter.x)) +((temp.z - CircleCenter.z) * (temp.z - CircleCenter.z));
		//distance = sqrt(distance);

		hitAWall = true;
		return lineP1 + (P2MinusP1 * u);
		//return distance;
		//return true;
	}
	/*Use LineP1 instead of LocalP1 because we want our answer in global
	space, not the circle's local space */

	
	else if (delta > 0){ // Two intersections
		double SquareRootDelta = sqrt(delta);

		double u1 = (-b + SquareRootDelta) / (2 * a);
		double u2 = (-b - SquareRootDelta) / (2 * a);
		Vec3 temp1 = P2MinusP1 * u1;  //((u1 * P2MinusP1.x) , (u1 * P2MinusP1.y) , (u1 * P2MinusP1.z));
		Vec3 temp2 = P2MinusP1 * u2;  //((u2 * P2MinusP1.x) , (u2 * P2MinusP1.y) , (u2 * P2MinusP1.z));

		Vec3 ret = lineP1 + (temp1-temp2);

		ret = ret / 2;
		
		hitAWall = true;
		return  ret; //LineP1 + (u2 * P2MinusP1);

	}

	hitAWall = false;
	return Vec3();
}


TargetList::closestWall TargetList::checkWalls(double velocity, double timeSinceLast){

	double dist = velocity * timeSinceLast;

	closestWall wall;

	wall.howFar = -1;
	wall.collisionPoint = Vec3();
	wall.hasBeenHit = false;

	Vec3 willBe = player->getFuturePosition(dist);

	Vec3 point1 = Vec3(-2640 , 0 , -2640);
	Vec3 point2 = Vec3(2640 , 0 , -2640); 
	
	Vec3 whereWeHit = circleLineCollision(point1, point2, willBe, dist);

	if(hitAWall){
		wall.collisionPoint = whereWeHit;
		Vec3 tmp = whereWeHit - willBe;
		wall.howFar = tmp.magnitude();
		wall.hasBeenHit = true;
	}

	point1 = point2;
	point2.set(2640, 0, 2640);

	whereWeHit = circleLineCollision(point1, point2, willBe, dist);

	if(hitAWall){
		wall.collisionPoint = whereWeHit;
		Vec3 tmp = whereWeHit - willBe;
		wall.howFar = tmp.magnitude();
		wall.hasBeenHit = true;
	}

	point1 = point2;
	point2.set(-2640, 0, 2640);

	whereWeHit = circleLineCollision(point1, point2, willBe, dist);

	if(hitAWall){
		wall.collisionPoint = whereWeHit;
		Vec3 tmp = whereWeHit - willBe;
		wall.howFar = tmp.magnitude();
		wall.hasBeenHit = true;
	}

	point1 = point2;
	point2.set(-2640, 0, -2640);

	whereWeHit = circleLineCollision(point1, point2, willBe, dist);

	if(hitAWall){
		wall.collisionPoint = whereWeHit;
		Vec3 tmp = whereWeHit - willBe;
		wall.howFar = tmp.magnitude();
		wall.hasBeenHit = true;
	}

	return wall;
}



/*
//TODO- old return type, delete later
bool TargetList::checkWalls(double velocity, double timeSinceLast){

	double dist = velocity * timeSinceLast;

	closestWall wall;

	wall.howFar = -1;

		Vec3 willBe = player->getFuturePosition(dist);

		Vec3 wall1((-5280/2) , 0 , (-5280/2));
		//Vec3 wall2((5280/2) , 100 , (-5280/2)); 
		Vec3 wall2((5280/2) , 0 , (-5280/2)); 
		//double distanceTo = circleLineCollision(wall1 , wall2, willBe);

		Vec3 hitPoint = circleLineCollision(wall1, wall2, willBe, dist);
		if(hitAWall){
	//		cout << "HIT WALL1" << endl;
		}

		wall1.set((5280/2), 0 , (5280/2));
		hitPoint = circleLineCollision(wall1, wall2, willBe, dist);
		if(hitAWall){
	//		cout << "HIT WALL2" << endl;
		}

		wall2.set((-5280/2) , 0 , (5280/2));
		hitPoint = circleLineCollision(wall1, wall2, willBe, dist);
		if(hitAWall){
	//		cout << "HIT WALL3" << endl;
		}

		wall1.set((-5280/2) , 0 , (-5280/2));
		hitPoint = circleLineCollision(wall1, wall2, willBe, dist);
		if(hitAWall){
	//		cout << "HIT WALL4" << endl;
		}

	return false;
}
*/
/*
	Runs through the list and draws all Targets

	-working
*/
void TargetList::DrawAll(Vec3 pos){
	Target *curr = head;

	for(int i=0; i < size; i++){
		curr->Draw(our_font, pos);
		curr = curr->getNext();
	}
}

/*

//TODO- remove later
//To be used in the godMode viewport, draws all targets as single points
//of the appropriate color.
//Not currently used (and not the same version as was in brennan's code)
void TargetList::DrawAsPoints(){
	Target *curr = head;
	Vec3 pos;

	for(int i = 0; i < size; i++){
		pos = curr->position;

		if(curr->isHit()) glColor3d(0, 1, 0);
		else glColor3d(1, 0, 0);

		glBegin(GL_POINTS);
		glVertex3d(pos.x, pos.y, pos.z);
		glEnd();

		curr = curr->getNext();
	}
}*/

/*
	Function to see if we've won the game or not.

	//TODO- recheck if working
*/
int TargetList::checkVictory(){
	bool winTotal = 0;
	Target *curr = head;

	for(int i = 0; i < size; i++){
		if(curr->isHit()) winTotal++;
		curr = curr->getNext();
	}

	return winTotal;
}

/*
bool TargetList::checkVictory(){
	bool win = true;
	Target *curr = head;

	for(int i = 0; i < size; i++){
		if(!curr->isHit()) win = false;
		curr = curr->getNext();
	}

	return win;
}
*/