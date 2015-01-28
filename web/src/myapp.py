#!/usr/bin/env python
# -*- coding: utf-8 -*-
import ConfigParser
import sqlite3


class BaseApp:

    def __init__(self):
        self.config = ConfigParser.ConfigParser()
        self.config.read("../conf/config.conf")
        self.db = sqlite3.connect(self.config.get("Sql", "Datafile"))

baseApp = BaseApp()
