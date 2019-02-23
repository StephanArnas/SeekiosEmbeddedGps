#include <tools/utils.h>

/* Regroupe un ensemble de fonctions utilitaires */

/* Répète une fonction jusqu'à ce que le nombre d'essais soit écoulés ou que la fonction a retourné "FUNCTION_SUCCESS" 
Note : la fonction doit retourner un int et ne pas avoir de paramètres.
Retourne FUNCTION_SUCCESS si la fonction passée en paramètre a marché, FUNCTION_FAILURE sinon
TODO : faire une fonction à paramètres variables qui les passerait potentiellement à la fonction à répéter */
int utils_repeatFunctionUntilSuccess(int (*callback)(void), int maxTries){
	int nbTries = 0;

	while((*callback)() != FUNCTION_SUCCESS && nbTries < maxTries){
		nbTries++;
	}

	if(nbTries >= maxTries){
		return FUNCTION_FAILURE;
	}

	return FUNCTION_SUCCESS;
}

/* Bubble sort asc */
void utils_sortUint16ArrayAsc(uint16_t array[], uint8_t arraySize)
{
	if(arraySize == 1)
	{
		return;
	}

	for(int i=0;i<arraySize;i++)
	{
		for(int j = 0;j<arraySize-1;j++)
		{
			if(array[j] > array[j+1])
			{
				uint16_t temp = array[j];
				array[j] = array[j+1];
				array[j+1] = temp;
			}
		}
	}
}