from random import randint
import subprocess

def update_text(num_people):
	if num_people == 1:
		return "'There is {} people in the view.'".format(num_people)
	else:
		return "'There are {} people in the view.'".format(num_people)

def audio_effect(num_people):
	text = update_text(num_people)
	subprocess.run(["gtts-cli", text, "--output", "people_audio.mp3"])
	subprocess.run(["mpg321", "people_audio.mp3", "-quiet"])
	return


# test code to see how well it works
def audio_display_test():
	while (True):
		num_people = randint(0,20)
		k = input()
		if k == "":
			audio_effect(num_people)
		else:
			break

	return