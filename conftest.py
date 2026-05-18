import pytest
import redis
from testcontainers.redis import RedisContainer

@pytest.fixture(scope="session")
def redis_container():
    with RedisContainer("redis:7.2-alpine") as container:
        yield container

@pytest.fixture(scope="function")
def redis_client(redis_container):
    host = redis_container.get_container_host_ip()
    port = redis_container.get_exposed_port(6379)
    client = redis.Redis(host=host, port=port, decode_responses=True)
    
    # Garantir que o DB esteja limpo antes de cada teste
    client.flushdb()
    yield client
    client.close()