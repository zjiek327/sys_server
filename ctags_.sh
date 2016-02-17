#!/bin/bash

ctags -R ./*
ctags --c-kinds=+px --c++-kinds=+px --fields=+iafksS --extra=+qf -R ./*
