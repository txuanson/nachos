[![HCMUS](https://www.hcmus.edu.vn/images/logo81.png)](https://www.hcmus.edu.vn/)
# Not Another Completely Heuristic Operating System
[![NachOS](https://slideplayer.com/7415498/24/images/slide_1.jpg)](https://en.wikipedia.org/wiki/Not_Another_Completely_Heuristic_Operating_System)
## Table of contents
- [💡 How to use this repository](https://github.com/txuanson/nachos#-how-to-use-this-repository)
- [📦 Setting up NachOS enviroment](https://github.com/txuanson/nachos#-setting-up-nachos-enviroment)
- [🔨 Implements](https://github.com/txuanson/nachos#-implements)
### 💡 How to use this repository
- Update, install git and make:
```sh
sudo apt-get update
sudo apt-get install gcc
sudo apt-get install g++
sudo apt-get install git
sudo apt-get install make
```
- Clone this repository:
```sh
# cd to your outer destination folder
git clone https://github.com/txuanson/nachos.git
```
- Go to the code folder:
```sh
cd nachos-3.4/code
```
- Build NachOS:
```sh
make
```
- Run
```sh
# Available values for {app}: createfile - echo - cat - copy - delete
./userprog/nachos -d -rs 1023 -x ./test/{app}
```
### 📦 Setting up NachOS enviroment
### 🔨 Implements
