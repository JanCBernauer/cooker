DROP table if exists comments cascade;

CREATE table comments
(
commentid serial,
runid int,
comment text,
constraint comments_pkey PRIMARY KEY (commentid)
) without OIDS; 

ALTER TABLE comments OWNER TO runinfo;