# RZV2L AI Library - Age and Gender detection

## Introduction

These application showcases the capability of deep neural networks to predict age-group and detect the gender of a person.
Image based gender classification and age classification applications are available in the respective folders identifiable with respective prefixes.

sample video on YouTube -[Age and gender detection demo video](https://youtu.be/-DpAGb7q4pM)

## Age Application details


### How to build the sample application

Please follow the below steps:

```
cd $work_dir
git clone https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary 
cd RZV2L_AiLibrary/05_Age_Gender_Detection/05_Age_classifier_img
make
```

## Running the application

1. Now, copy the `RZV2L_AiLibrary` directory to the board (/home/root/).
2. Please follow these steps:


### Application with image input


1)Enter the relative path for the test_image with respect to the directory of the executable when prompted
2)Along with the test_image path pass the test_image dimension in the order width X height 
3)For sample test images the image dimensions are provided in the 'test_images_dimensions.txt'

#### Example:
```
cd /home/root/RZV2L_AiLibrary 
cd 05_Age_Gender_Detection/05_Age_classifier_img/exe
./05_age_img_app
# When prompted for the image path
./sample.bmp
```

## Gender Application details


### How to build the sample application

Please follow the below steps:

```
cd $work_dir
git clone https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary 
cd RZV2L_AiLibrary/05_Age_Gender_Detection/05_Gender_classifier_img
make
```

## Running the application

1. Now, copy the `RZV2L_AiLibrary` directory to the board (/home/root/).
2. Please follow these steps:


### Application with image input


1)Enter the relative path for the test_image with respect to the directory of the executable when prompted
2)Along with the test_image path pass the test_image dimension in the order width X height 
3)For sample test images the image dimensions are provided in the 'test_images_dimensions.txt'

#### Example:
```
cd /home/root/RZV2L_AiLibrary 
cd 05_Age_Gender_Detection/05_Gender_classifier_img/exe
./05_gender_img_app
# When prompted for the image path
./sample.bmp
```


### Known issues:
1. [ERROR] Image buffer address is NULL : This error suggests that the input path to the image is improper. Verify the path, check whether an image available in the path.
2. Segmentation fault : If you are running the application in image mode, beware of the image dimensions entered. If entered image width or height is larger than the actual image dimensions, then a segmentation fault will occur.
3. Improper output : If you are running the application in image mode, beware of the image dimensions entered. If entered image width or height is smaller than the actual image dimensions, then improper or unexpected outputs will be observed.
4. permission denied - This error may occur if executable file does not have execution permission. Use this command - `chmod 777 executable_filename` to assign proper permissions.

