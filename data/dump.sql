CREATE TABLE user (
id        INTEGER PRIMARY KEY,
username  TEXT UNIQUE NOT NULL,
password  TEXT NOT NULL,
utype     INTEGER,
regdate   DATE);

INSERT INTO user (id, username, password) values (1, 'paradox', '6b3a55e0261b0304143f805a24924d0c1c44524821305f31d9277843b8a10f4e');
INSERT INTO user (id, username, password) values (2, 'gufo',    '6b3a55e0261b0304143f805a24924d0c1c44524821305f31d9277843b8a10f4e');
INSERT INTO user (id, username, password) values (3, 'daniele', '6b3a55e0261b0304143f805a24924d0c1c44524821305f31d9277843b8a10f4e');
INSERT INTO user (id, username, password) values (4, 'viralex', '6b3a55e0261b0304143f805a24924d0c1c44524821305f31d9277843b8a10f4e');

