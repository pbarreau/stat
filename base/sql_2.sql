 insert into DistriCombi (id_com,tip) select id, tip  from (SELECT lstcombi.id,lstcombi.tip,t2.id_poids
FROM lstcombi
LEFT JOIN (select * from (select analyses.id from analyses where analyses.id_poids = 107) as t1 left join analyses on t1.id = analyses.id+1) as t2 ON lstcombi.id = t2.id_poids)as t1 GROUP BY tip having ((t1.id=t1.id_poids)or (t1.id_poids is null))  order by t1.id asc;

insert into DistriCombi (id_com,tip) select id, tip  from lstcombi;

select * from (select analyses.id from analyses where analyses.id_poids = 107);

select * from (select analyses.id from analyses where analyses.id_poids = 107) as t1 left join analyses on t1.id = analyses.id;

select * from (select analyses.id from analyses where analyses.id_poids = 107) as t1 left join tirages on t1.id = tirages.id;

select id, count(id_poids) as T,tip  from (SELECT lstcombi.id,lstcombi.tip,t2.id_poids
FROM lstcombi
LEFT JOIN (select * from (select analyses.id from analyses where analyses.id_poids = 107) as t1 left join analyses on t1.id = analyses.id+1) as t2 ON lstcombi.id = t2.id_poids)as t1 GROUP BY tip having ((t1.id=t1.id_poids)or (t1.id_poids is null))  order by t1.id asc;

select id, count(id_poids) as T  from (SELECT lstcombi.id,t2.id_poids
FROM lstcombi
LEFT JOIN (select * from (select analyses.id from analyses where analyses.id_poids = 107) as t1 left join analyses on t1.id = analyses.id+1) as t2 ON lstcombi.id = t2.id_poids)as t1 GROUP BY id having ((t1.id=t1.id_poids)or (t1.id_poids is null))  order by t1.id asc;


select sum(T) from (select id, count(id_poids) as T  from (SELECT lstcombi.id,lstcombi.tip,t2.id_poids
FROM lstcombi
LEFT JOIN (select * from (select analyses.id from analyses where analyses.id_poids = 107) as t1 left join analyses on t1.id = analyses.id+2) as t2 ON lstcombi.id = t2.id_poids)as t1 GROUP BY tip having ((t1.id=t1.id_poids)or (t1.id_poids is null))  order by t1.id asc);


select id  from (SELECT lstcombi.id,t2.id_poids
FROM lstcombi
LEFT JOIN (select * from (select analyses.id from analyses where analyses.id_poids = 107) as t1 left join analyses on t1.id = analyses.id+2) as t2 ON lstcombi.id = t2.id_poids)as t1 order by t1.id asc;

select * from  (
select id  from (SELECT lstcombi.id,t2.id_poids
FROM lstcombi
LEFT JOIN (select * from (select analyses.id from analyses where analyses.id_poids = 107) as t1 left join analyses on t1.id = analyses.id+2) as t2 ON lstcombi.id = t2.id_poids)as t1 order by t1.id asc
)as letout left join (select * from tirages);