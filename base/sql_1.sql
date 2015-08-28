select tip, count(t1.id_poids)as tot from (SELECT lstcombi.id,lstcombi.tip,analyses.id_poids
FROM lstcombi
LEFT JOIN analyses ON lstcombi.id = analyses.id_poids)as t1 GROUP BY tip having ((t1.id=t1.id_poids) or (t1.id_poids is null))  order by tot  desc, tip desc;

select tip from lstcombi where (id=1);

select * from analyses where analyses.id_poids = 126;

select * from (select analyses.id from analyses where analyses.id_poids = 126) as t1 left join tirages on t1.id = tirages.id+1;

select * from (select analyses.id from analyses where analyses.id_poids = 126) as t1 left join analyses on t1.id = analyses.id+1;

create table DistriCombi (id INTEGER PRIMARY KEY, id_com int, tip text, s1 int, s2 int, p1 int, p2 int);

drop table DistriCombi;