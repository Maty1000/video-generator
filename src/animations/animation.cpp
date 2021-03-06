#include "animation.h"

animation::animation(vector<doubleCSSprop> aviableProps, animationType type, double requestedStrength) : type(type)
{
	doubleCSSprop* appearingProp = NULL;

	// At least one appearing/disappearing CSS property is needed when type is not accenting.
	if (type != animationType::accenting) {
		vector<doubleCSSprop> appearingProps;

		copy_if(aviableProps.begin(), aviableProps.end(), back_inserter(appearingProps), [](doubleCSSprop prop) {
			return prop.badVisibility.size() != 0;
		});

		if (appearingProps.size() == 0) {
			throw new logic_error("Requested appearing/disappearing CSS property, but in aviableProps isn't any property with badVisibility.");
		};

		unsigned short appearingPropIndex = rand() % appearingProps.size();
		appearingProp = new doubleCSSprop(appearingProps[appearingPropIndex]);

		appearingProp->animate(type);
		add(appearingProp);
	}

	// Props, where decorative isn't NULL (and also exclude appearingProp to avoid its overriding).
	vector<doubleCSSprop> accentingProps;
	copy_if(aviableProps.begin(), aviableProps.end(), back_inserter(accentingProps), [appearingProp](doubleCSSprop prop) {
		return prop.decorative != NULL 
			&& (appearingProp != NULL ? (prop.name != appearingProp->name) : true); // Do not repeat appearingProp.
	});

	// Add properties until the requestedStrength is reached.
	while (getStrength() < requestedStrength && accentingProps.size() != 0) {
		unsigned short toAddIndex = rand() % accentingProps.size();
		doubleCSSprop toAdd = doubleCSSprop(accentingProps[toAddIndex]);
		vector<doubleCSSprop>::iterator myBegin = accentingProps.begin();
		accentingProps.erase(accentingProps.begin() + toAddIndex);

		toAdd.animate(
			animationType::accenting,
			// Set strength to rearmining strength or to random number between 0 and 1
			min(
				helperFunctions::doubleRandBetween(range<double>(0, 1)),
				requestedStrength - getStrength()));

		if (type == animationType::appearing) {
			toAdd.animation->animation.swap();
		}

		add(new doubleCSSprop(toAdd));
	}
}

// TODO: Avoid duplicate prop assignments in output.
const string animation::generate(string name)
{
	string out = "@keyframes " + name + "{from{";
	for (iterator it = this->begin(); it != end(); ++it) {
		out += (*it)->generateBegin();

		vector<iterator> propsWithSameName = getPropsWithSameName(it);
		for (vector<iterator>::iterator it2 = propsWithSameName.begin(); it2 != propsWithSameName.end(); ++it2) {
			out += " " + (**it2)->generateValue(*(**it2)->animation->animation.min);
		}

		out += ";";
	}

	if (type == animationType::appearing) {
		out += "opacity:0;}1%{opacity:1;";
	}

	if (type == animationType::disappearing) {
		out += "}99%{opacity:1;";
	}

	out += "}to{";
	for (iterator it = this->begin(); it != end(); ++it) {
		out += (*it)->generateBegin();

		vector<iterator> propsWithSameName = getPropsWithSameName(it);
		for (vector<iterator>::iterator it2 = propsWithSameName.begin(); it2 != propsWithSameName.end(); ++it2) {
			out += " " + (**it2)->generateValue(*(**it2)->animation->animation.max);
		}

		out += ";";
	}

	if (type == animationType::disappearing) {
		out += "opacity:0;";
	}

	out += "}}";
	return out;
}

void animation::add(CSSprop *toAdd)
{
	push_back(toAdd);
}

const vector<animation::iterator> animation::getPropsWithSameName(iterator nameToFind)
{
	vector<iterator> out;
	for (iterator it = this->begin(); it != end(); ++it) {
		if ((*it)->name == (*nameToFind)->name) { // Name is same
			out.push_back(it);
		}
	}
	return out;
}
