CREATE TABLE user (
id        INTEGER PRIMARY KEY,
username  TEXT UNIQUE NOT NULL,
password  TEXT NOT NULL,
utype     INTEGER,
regdate   DATE);

INSERT INTO user (id, username, password, utype) values (1, 'paradox', '5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8', 0);
INSERT INTO user (id, username, password, utype) values (2, 'gufo',    '5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8', 0);
INSERT INTO user (id, username, password, utype) values (3, 'daniele', '5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8', 2);
INSERT INTO user (id, username, password, utype) values (4, 'viralex', '5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8', 1);
INSERT INTO user (id, username, password, utype) values (5, 'alec',    '5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8', 2);
INSERT INTO user (id, username, password, utype) values (6, 'the_doctor',  'a5ce2bb2a811bea4c3321b4e999185ffcc93f1a1c3455f72c2ed5d3b70c33e41', 2);
