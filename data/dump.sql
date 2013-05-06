CREATE TABLE user (
id        INTEGER PRIMARY KEY,
username  TEXT UNIQUE NOT NULL,
password  TEXT NOT NULL,
utype     INTEGER,
regdate   DATE);

INSERT INTO user (id, username, password) values (1, 'paradox', 'password');
INSERT INTO user (id, username, password) values (2, 'gufo',    'password');
INSERT INTO user (id, username, password) values (3, 'daniele', 'password');
INSERT INTO user (id, username, password) values (4, 'viralex', 'password');

