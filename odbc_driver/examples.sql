DROP TABLE IF EXISTS data;
CREATE TABLE data (key VARCHAR(255) PRIMARY KEY, value VARCHAR(1023));
insert into data values ('key','value');
insert into data values ('abc','123');
select * from data
