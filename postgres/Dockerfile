from postgres:11.1-alpine

COPY init.sql /docker-entrypoint-initdb.d

CMD [   "postgres", \
        "-c", "max_connections=1500", \
        "-c", "shared_buffers=2GB", \
        "-c", "effective_cache_size=6GB", \
        "-c", "maintenance_work_mem=512MB", \
        "-c", "checkpoint_completion_target=0.9", \
        "-c", "wal_buffers=16MB", \
        "-c", "default_statistics_target=100", \
        "-c", "random_page_cost=1.1", \
        "-c", "effective_io_concurrency=200", \
        "-c", "work_mem=699kB", \
        "-c", "min_wal_size=2GB", \
        "-c", "max_wal_size=4GB", \
        "-c", "max_worker_processes=4", \
        "-c", "max_parallel_workers_per_gather=2", \
        "-c", "max_parallel_workers=4" \
]

