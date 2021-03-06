#include "video.h"

const string video::generate() {
	cout << "Analyzing music, please wait..." << endl;
	if (beats.size() == 0) {
		beats = helperFunctions::getBeats(style.musicURL, 1.5, getNeededTransitionsCount(), true);
	}

	assignBeatsToWords(&beats);
	return "<!DOCTYPE html><html lang='en'><head><meta charset='utf - 8'><title>Video</title><script>var audio=new Audio('" + style.getEscapedMusicURL() + "');audio.play()</script><style>" + style.baseCSS + getKeyframes() + "</style></head><body>" + generateHTML(beats) + "</body></html><!-- Generated with Maty1000's video generator. -->";
}

const string video::getKeyframes()
{
	return keyframesCode == "" ? generateKeyframes() : keyframesCode;
}

const string video::generateKeyframes(short startFrom)
{
	string out;

	// Appearing animations
	unsigned short i = startFrom;
	for (auto currentWord : words) {

		// If there are more words than beats.
		if (currentWord.correspondingBeat == beats.end() - 1)
			break;

		animation anim(doubleCSSprop::props, animationType::appearing, style.animationStrength * currentWord.correspondingBeat->strength);
		out += anim.generate("a" + to_string(i));
		i++;
	}

	// Disappearing animations
	i = startFrom;
	for (auto currentWord : words) {

		// If there are more words than beats.
		if (currentWord.correspondingBeat == beats.end() - 1)
			break;

		animation anim(doubleCSSprop::props, animationType::disappearing, style.animationStrength * currentWord.correspondingBeat->strength);
		out += anim.generate("d" + to_string(i));
		i++;
	}

	keyframesCode = out;
	return out;
}

unsigned short int video::getNeededTransitionsCount()
{
	return words.size();
}

video::video(string text) {
	setText(text);
}

void video::setText(string text)
{
	vector <string> ::iterator it;
	vector <string> textOfWords = helperFunctions::splitStringByDelimiters(text, { " " });
	unsigned short i = 0;

	for (it = textOfWords.begin(); it != textOfWords.end(); ++it, ++i) {
		if (words.size() > i)
			words[i].text = *it; // Overwrite value
		else
			words.push_back(*it); // Add value
	}

	// Remove extra NULLs.
	if (words.size() > textOfWords.size())
		words.erase(words.begin() + textOfWords.size(), words.end());
}

const string video::generateHTML(
	beatGroup& beats,
	string element,
	string appearingAnimIdPrefix, string appearingAnimIdPostfix,
	string disappearingAnimIdPrefix, string disappearingAnimIdPostfix,
	short startFrom) {

	string out;
	unsigned short i = startFrom;

	for (vector<word>::iterator currentWord = words.begin(); currentWord != words.end(); ++currentWord) {

		// If there are more words than beats.
		if (currentWord->correspondingBeat == beats.end() - 1)
			break;


		double currentAnimDuration = currentWord->correspondingBeat->time;

		// Validate currentWord.correspondingBeat-1 only if it isn't at beggining of the vector.
		if (currentWord->correspondingBeat != beats.end()) {
			currentAnimDuration -= (currentWord->correspondingBeat + 1)->time;
		}
		currentAnimDuration = abs(currentAnimDuration);
		currentAnimDuration /= 2;

		out += generateHTMLElem(currentWord->text, "div",
			generateAnimationCSS(
				{
					animationCSS(   appearingAnimIdPrefix + to_string(i) +    appearingAnimIdPostfix, currentAnimDuration, animationCSS::fillMode::backwards, animationCSS::easing::type::out, currentWord->correspondingBeat->time),
					animationCSS(disappearingAnimIdPrefix + to_string(i) + disappearingAnimIdPostfix, currentAnimDuration, animationCSS::fillMode:: forwards, animationCSS::easing::type::in,  currentWord->correspondingBeat->time + currentAnimDuration)
				}) + ";font-size: " + helperFunctions::doubleToString(currentWord->correspondingBeat->strength * 250 + 100) + "%");
		i++;
	}
	return out;
}

const string video::generateHTMLElem(string content, string element, string style)
{
	return "<" + element + " style='" + style + "'>" + content + "</" + element + ">";
}

const string video::generateAnimationCSS(vector<animationCSS> anims)
{
	string out = "animation: ";
	for (auto anim : anims)
		out += anim.generate() + ",";
	return out.substr(0, out.size() - 1); // Remove extra colon from end
}

void video::assignBeatsToWords(beatGroup* beats)
{
	beatGroup::iterator currentBeat = beats->begin();
	for (
		vector<word>::iterator currentWord = words.begin();
		currentWord != words.end() && currentBeat != beats->end();
		currentWord++, currentBeat++) {

		currentWord->correspondingBeat = currentBeat;
	}
}
