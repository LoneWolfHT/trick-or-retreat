#ifndef LD45_UTIL
#define LD45_UTIL

#include <stdlib.h>
#include <SFML/Graphics.h>
#include <math.h>

#define V2F (sfVector2f)

int mrand(n1, n2)
int n1, n2;
{
	int out = -1;

	++n1;
	++n2;

	while (out < n1)
		out = rand() % n2+1;

	return(out-1);
}

sfVector2f v2fMultiply(sfVector2f v1, double x)
{
	return(V2F{v1.x * x, v1.y * x});
}

sfVector2f v2fSubtract(sfVector2f v1, sfVector2f v2)
{
	return(V2F{v1.x - v2.x, v1.y - v2.y});
}

sfVector2f v2fAdd(sfVector2f v1, sfVector2f v2)
{
	return(V2F{v1.x + v2.x, v1.y + v2.y});
}

sfVector2f v2fDivide(sfVector2f v1, double x)
{
	return(V2F{v1.x / x, v1.y / x});
}

sfVector2f v2fDir(sfVector2f v1, sfVector2f v2)
{
	sfVector2f output = v2fSubtract(v2, v1);

	return(v2fDivide(output, hypot(output.x, output.y)));
}

double v2fDist(sfVector2f v1, sfVector2f v2)
{
	sfVector2f output = v2fSubtract(v2, v1);

	return(sqrt(pow(output.x, 2) + pow(output.y, 2)));
}

int getsize(int num)
{
	int count = 0;

	while ((num = num / 10) > 0)
		++count;

	return(count);
}

int getlen(string)
char string[];
{
	int i;

	for (i = 0; string[i] != '\0'; ++i);

	return(i);
}

int find(s1, s2)
char *s1, *s2;
{
	int i;

	for (i=0; i <= getlen(s1); ++i)
	{
		if (s1[i] == s2[0])
		{
			for (int a = 1; a < getlen(s2); ++a)
			{
				if (s1[i+a] != s2[i+a])
					break;
			}
			return(i);
		}
	}

	return(-1);
}

#endif
