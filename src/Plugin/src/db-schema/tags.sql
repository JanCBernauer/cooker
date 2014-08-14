DROP table if exists tagnames cascade;

CREATE table tagnames
(
tagid serial,
tagname text,
constraint tagnames_pkey PRIMARY KEY (tagid)
) without OIDS; 

ALTER TABLE tagnames OWNER TO runinfo;