# TestDriver
Здесь работаю над тестерами для лабораторных работ по дисциплине Программирование ФИТ НГУ.

Ставьте звезды, создавайте issue, делайте пул-реквесты :-)

Если исходный код тестеров обновлялся менее 6 месяцев назад, то
архивы с исполняемыми файлами находятся по ссылке в разделах *Image: Visual Studio 2015* -> *Artifacts* и *Image: Ubuntu* -> *Artifacts*

https://ci.appveyor.com/project/Evgueni-Petrov-aka-espetrov/testdriver

Для компиляции тестов под убунту можно использовать такой докер файл:

``
FROM ubuntu:18.04 as build-env \n
RUN apt-get update && apt-get -y install gcc git
``
