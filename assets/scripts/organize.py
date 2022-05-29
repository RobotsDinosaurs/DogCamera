#pylint:disable=no-member

import os
import shutil

path_of_the_directory = '/home/ubuntu/ESP32/assets/frontyard/'

if not os.path.exists(path_of_the_directory):
    os.makedirs(path_of_the_directory)

object = os.scandir(path_of_the_directory)
print("Scanning files in '% s':" % path_of_the_directory)
for n in object :
    if n.is_file():
        print(n.name)
        new_pic_folder = n.name[0:8] + '/'
        
        if not os.path.exists(path_of_the_directory + new_pic_folder):
            os.makedirs(path_of_the_directory + new_pic_folder)   
        shutil.move(path_of_the_directory + n.name, path_of_the_directory + new_pic_folder + n.name)
        
object.close()
