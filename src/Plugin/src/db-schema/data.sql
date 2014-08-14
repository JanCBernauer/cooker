DROP table if exists rundata cascade;

CREATE table rundata
(
dataid serial,
timestamp timestamp,
runid int,
tagid int,
value double precision,
unique (runid,tagid),
constraint rundata_pkey PRIMARY KEY (dataid)
) without OIDS; 

ALTER TABLE rundata OWNER TO runinfo;


DROP table if exists detectorFlag cascade;

CREATE table detectorFlag
(
dataid serial,
timestamp timestamp,
runid int,
tagid int,
value int,
unique (runid,tagid),
constraint detectorFlag_pkey PRIMARY KEY (dataid)
) without OIDS; 

ALTER TABLE detectorFlag OWNER TO runinfo;


